#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
#include <libusb-1.0/libusb.h>
#include <scsi/sg.h>
#include <libudev.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// Create Timing Fingerprint and store timing.txt
void create_fingerprint(char* device_path){
        FILE *output = fopen("timing.txt", "w");
        if(output == NULL){
                fprintf(stderr, "Error opening log file");
                exit(1);
        }
        int fd = open(device_path, O_RDWR);
        if(fd<0){
                fprintf(stderr, "Error opening device file");
                exit(1);
        }

        sg_io_hdr_t io_hdr;
        unsigned char cdb[16];  // Command block
        int num_requests = 2900;
        unsigned long lbas[] = {0x0, 0x140000, 0x280000, 0x3c0000, 0x500000, 0x640000};
        int num_lbas = sizeof(lbas) / sizeof(lbas[0]);
        //int read_sizes[] = {16*1024, 32*1024, 64*1024};
        int read_sizes[] = {8*1024, 16*1024, 32*1024};
        int num_sizes = sizeof(read_sizes) / sizeof(read_sizes[0]);
        unsigned char data_buffer[65536];
        struct timespec start_time, end_time;
        uint64_t start, end;

        for(int i=0 ; i<num_requests ; i++){
                // Randomly choose read size and logical blocks
                unsigned long lba = lbas[rand()%num_lbas];
                int read_size = read_sizes[rand()%num_sizes];

                memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
                memset(cdb, 0, sizeof(cdb));

                // Setup Command block
                cdb[0] = 0x28;          // SCSI READ opcode
                cdb[1] |= (1 << 4);     // Set the Direct IO (DIO) flag
                cdb[1] |= (1 << 3);     // Set the Disable Page Out (DPO) flag
                cdb[1] |= (1 << 2);     // Set the Force Unit Access (FUA) flag
                cdb[2] = (lba >> 24) & 0xFF;
                cdb[3] = (lba >> 16) & 0xFF;
                cdb[4] = (lba >> 8) & 0xFF;
                cdb[5] = lba & 0xFF;
                cdb[7] = (read_size >> 8) & 0xFF;
                cdb[8] = read_size & 0xFF;

                io_hdr.interface_id = 'S';
                io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
                io_hdr.cmdp = cdb;
                io_hdr.cmd_len = sizeof(cdb);
                io_hdr.dxferp = data_buffer;
                io_hdr.dxfer_len = read_size;

                // Record start time
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                asm volatile ( "mrs %0, cntvct_el0" : "=r" (start));

                // Send the SCSI command to the device
                if(ioctl(fd, SG_IO, &io_hdr) < 0){
                        fprintf(stderr, "Error sending SCSI command ... \n");
                        //close(fd);
                        //exit(1);
                        continue;
                }

                // Record end time
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                asm volatile ( "mrs %0, cntvct_el0" : "=r" (end));
                
		// Store the time
                fprintf(output, "read size: %d, logical block: 0x%06x, timing: %09ld\n",read_size, lba, end-start);
        }
        close(fd);
        fclose(output);
        printf("finish creating timing fingerprint ...\n");
}


// Create Timing Fingerprint and store in a specific file name
void create_fingerprint_filename(char* device_path, char* s){
	//FILE *output = fopen("timing.txt","w");
	FILE *output = fopen(s, "w");
	if(output == NULL){
		fprintf(stderr, "Error opening log file");
		exit(1);
	}

	printf("file name: %s\n", s);
	int fd = open(device_path, O_RDWR);
	if(fd<0){
		fprintf(stderr, "Error opening device file");
		exit(1);
	}
	
	sg_io_hdr_t io_hdr;
	unsigned char cdb[16];	// Command block

	int num_requests = 2900;
	unsigned long lbas[] = {0x0, 0x140000, 0x280000, 0x3c0000, 0x500000, 0x640000};
	int num_lbas = sizeof(lbas) / sizeof(lbas[0]);
	//int read_sizes[] = {16*1024, 32*1024, 64*1024};
	int read_sizes[] = {8*1024, 16*1024, 32*1024};
	int num_sizes = sizeof(read_sizes) / sizeof(read_sizes[0]);
	unsigned char data_buffer[65536];

	struct timespec start_time, end_time;
	uint64_t start, end;

	printf("Start for loop ...");
	for(int i=0 ; i<num_requests ; i++){
		// Randomly choose read size and logical blocks
		unsigned long lba = lbas[rand()%num_lbas];
		int read_size = read_sizes[rand()%num_sizes];
		if(i%10==0){
			printf("\n%d",i);
		}
		else{
			printf(".");
		}
		//printf("%d ----- read size : %d ----- logical blocks : 0x%06x\n",i, read_size, lba);

		memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
		memset(cdb, 0, sizeof(cdb));

		// Setup Command block
		cdb[0] = 0x28;		// SCSI READ opcode
		cdb[1] |= (1 << 4);	// Set the Direct IO (DIO) flag
		cdb[1] |= (1 << 3);	// Set the Disable Page Out (DPO) flag
		cdb[1] |= (1 << 2);	// Set the Force Unit Access (FUA) flag
		cdb[2] = (lba >> 24) & 0xFF;
		cdb[3] = (lba >> 16) & 0xFF;
		cdb[4] = (lba >> 8) & 0xFF;
		cdb[5] = lba & 0xFF;
		cdb[7] = (read_size >> 8) & 0xFF;
		cdb[8] = read_size & 0xFF;

		io_hdr.interface_id = 'S';
		io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
		io_hdr.cmdp = cdb;
		io_hdr.cmd_len = sizeof(cdb);
		io_hdr.dxferp = data_buffer;
		io_hdr.dxfer_len = read_size;
		
		// Record start time
		//clock_gettime(CLOCK_MONOTONIC, &start_time);
		//asm volatile("" : : : "memory");
		//start = rdtsc();
		/*asm volatile ( "rdtsc\n\t"
			"shl $32, %%rdx\n\t"
			"or %%rdx, %0"
			: "=a" (start)
			:
			: "rdx");
		*/
		asm volatile ( "mrs %0, cntvct_el0" : "=r" (start));

		// Send the SCSI command to the device
		if(ioctl(fd, SG_IO, &io_hdr) < 0){
			fprintf(stderr, "Error sending SCSI command ... \n");
			//close(fd);	
			//exit(1);
			continue;
		}

		// Record end time
		//clock_gettime(CLOCK_MONOTONIC, &end_time);
		//printf("  time: %09ld\n",end_time.tv_nsec - start_time.tv_nsec);
		//asm volatile("" : : : "memory");
		//end = rdtsc();
		/*asm volatile ( "rdtsc\n\t"
			"shl $32, %rdx\n\t"
			"or %rdx, %0"
			: "=a" (end)
			:
			: "rdx");
		*/
		asm volatile ( "mrs %0, cntvct_el0" : "=r" (end));
		//printf("CPU cycles elapsed: %lu\n",end-start);

		// Store the time
		//fprintf(output, "read size: %d, logical block: 0x%06x, timing: %09ld\n",read_size, lba, end_time.tv_nsec-start_time.tv_nsec);
		fprintf(output, "read size: %d, logical block: 0x%06x, timing: %09ld\n",read_size, lba, end-start);
	}
	close(fd);
	fclose(output);
	printf("finish creating timing fingerprint ...\n");
}

static __inline__ uint64_t rdtsc(void){
	uint32_t lo, hi;
	uint64_t msr;
	//__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	/*asm volatile (
		"rdtsc" 
		: "=a" (lo), "=d" (hi) 
		: : "memory"
	);*/

	asm volatile (
		"rdtsc\n\t"
		: "=a" (lo), "=d" (hi)
		: 
		:
	);
	/*
	asm volatile ("rdtsc\n\t"
		"shl $32, %%rdx\n\t"
		"or %%rdx, %0"
		: "=a" (msr)
		:
		: "rdx"		
	);*/
	return ((uint64_t)hi << 32) | lo;
	//return msr;
}

// Print usb device
void print_usb_device(libusb_device *device){
        struct libusb_device_descriptor descriptor;
        int ret = libusb_get_device_descriptor(device, &descriptor);
        if(ret<0){
                fprintf(stderr, "libusb_get_device_descriptor error: %s\n",libusb_error_name(ret));
                exit(1);
        }
        libusb_device_handle *handle;
        libusb_open(device, &handle);
        char manufacturer[256], product[256];
        libusb_get_string_descriptor_ascii(handle, descriptor.iManufacturer, (unsigned char*)manufacturer, sizeof(manufacturer));
        libusb_get_string_descriptor_ascii(handle, descriptor.iProduct, (unsigned char*)product, sizeof(product));
        printf(" Device Product name: %s\n",product);
        printf("  Manufacturer: %s\n",manufacturer);
        printf("  Vendor ID: 0x%04x\n", descriptor.idVendor);
        printf("  Product ID: 0x%04x\n", descriptor.idProduct);
        printf("  Device class: %d\n", descriptor.bDeviceClass);
        printf("  Device subclass: %d\n", descriptor.bDeviceSubClass);
        printf("  Device Protocol: %d\n", descriptor.bDeviceProtocol);

}
