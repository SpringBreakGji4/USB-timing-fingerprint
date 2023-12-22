#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <libusb-1.0/libusb.h>
#include <pcap.h>
#include "func.c"

void usb_events(libusb_context *ctx, ssize_t pre_count);
void capture_callback(unsigned char* user_data, const struct pcap_pkthdr *pkthdr, const unsigned char* packet);

int main(int argc, char** argv){
	char* device_path = argv[1];
	printf("USB device path: %s\n",device_path);
	libusb_context *ctx = NULL;
	int rc = libusb_init(&ctx);
	if(rc<0){
		fprintf(stderr, "libusb_init error: %s\n", libusb_error_name(rc));
		return 1;
	}
	
	// Print out the current USB device
	libusb_device **list;
	ssize_t count = libusb_get_device_list(ctx, &list);
	if(count<0){
		fprintf(stderr, "libusb_get_device_list error\n");
		libusb_exit(ctx);
		return 1;
	}
	printf("Current USB device: \n");
	for(ssize_t i=0 ; i<count ; i++){
		printf("\nDevice: %d ------------------------------\n",i);
		print_usb_device(list[i]);
	}
	libusb_free_device_list(list, 1);
	
	create_fingerprint(device_path);

	// Capture USB packet using pcap
	pcap_t *handle;			
	char errBuf[PCAP_ERRBUF_SIZE];	// Error msg buffer
	pcap_if_t* interfaces, * temp;
	
	// find all device interfaces
	if(pcap_findalldevs(&interfaces, errBuf) == -1){
		fprintf(stderr, "pcap_findalldevs error: %s\n",errBuf);
		exit(1);
	}
	
	// print out all interfaces
	printf("Current device interfaces: \n");
	int index=0;
	for(temp=interfaces ; temp ; temp=temp->next){
		printf(" %d: %s\n",index++, temp->name);
	}

	// Start capturing packet
	handle = pcap_open_live("usbmon1", BUFSIZ, 1, 1000, errBuf);	// the first reference is the interface we want to listen
	if(handle == NULL){
		fprintf(stderr, "pcap_open_live error: %s\n",errBuf);
		exit(1);
	}

	const char* filter_expression = "";	// we can use filter to filter out specific packet e.g. "port 80"
	struct bpf_program fp;

	if(pcap_compile(handle, &fp, filter_expression, 0, PCAP_NETMASK_UNKNOWN) == -1){
		fprintf(stderr, "pcap_compile error: %s\n",pcap_geterr(handle));
		exit(1);
	}

	if(pcap_setfilter(handle, &fp) == -1){
		fprintf(stderr, "pcap_setfilter error: %s\n",pcap_geterr(handle));
		exit(1);
	}

	if(pcap_loop(handle, 0, capture_callback, NULL) < 0){
		fprintf(stderr, "pcap_loop error: %s\n", pcap_geterr(handle));
	}



	//usb_events(ctx);	// Listen to any incoming plug-in device

	libusb_exit(ctx);
	
	return 0;
}

// Process USB packet
void capture_callback(unsigned char* user_data, const struct pcap_pkthdr *pkthdr, const unsigned char* packet){
	printf("Packet Timestamp: %ld.%06ld\n",pkthdr->ts.tv_sec, pkthdr->ts.tv_usec);
	printf("       Length: %d\n",pkthdr->len);
	printf("--------------------\n");
	for(int i=0 ; i<pkthdr->len ; i++){
		printf("%02x", packet[i]);
		if((i+1)%16 == 0){
			printf("\n");
		}
	}
	printf("\n---------------------\n");
	
	// Implement packet filter
	
}

// Listen to any incoming plug-in device
void usb_events(libusb_context *ctx, ssize_t pre_count){
	while(1){
		printf("--------Listening to any incoming device ... --------\n");
		// (black/whitelisting)
		int rc = libusb_handle_events(ctx);
		if(rc<0){
			fprintf(stderr, "libusb_handle_events error: %s\n", libusb_error_name(rc));
			break;
		}
		libusb_device **list;
		ssize_t count = libusb_get_device_list(ctx, &list);
		if(count<0){
			fprintf(stderr, "libusb_get_device_list error\n");
			libusb_exit(ctx);
			exit(1);	
		}
		/*
		for(ssize_t i=0 ; i<count ; i++){
			print_usb_device(list[i]);
		}
		*/
		if(count > pre_count){
			printf("\nNew Device ------------------------------\n");
			print_usb_device(list[0]);
			//create_fingerprint();
		}

	}
}
