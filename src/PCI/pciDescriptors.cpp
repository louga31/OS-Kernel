#include "pci.h"
#include <types.h>
#include <cstr.h>

namespace PCI {
	const char* DeviceClasses[] {
		"Unclassified",
		"Mass Storage Controller",
		"Network Controller",
		"Display Controller",
		"Multimedia Controller",
		"Memory Controller",
		"Bridge Device",
		"Simple Communication Controller",
		"Base System Peripheral",
		"Input Device Controller",
		"Docking Station",
		"Processor",
		"Serial Bus Controller",
		"Wireless Controller",
		"Intelligent Controller",
		"Satellite Communication Controller",
		"Encryption Controller",
		"Signal Processing Controller",
		"Processing Accelerator",
		"Non Essential Instrumentation"
	};

	const char* GetVendorName(uint16_t vendorID) {
		switch (vendorID) {
			case 0x8086:
				return "Intel Corp";
			case 0x1022:
				return "AMD";
			case 0x10DE:
				return "Nvidia Corporation";
			default:
				return to_hstring(vendorID);
		}
	}

	const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID) {
		uint32_t ID = (vendorID << 16) | deviceID;
		switch (ID){
			case 0x80861901:
				return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor PCIe Controller (x16)";
			case 0x80861903:
				return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor Thermal Subsystem";
			case 0x80862822:
				return "SATA Controller [RAID mode]";
			case 0x80862918:
				return "LPC Interface Controller";
			case 0x80862922:
				return "6 port SATA Controller [AHCI mode]";
			case 0x80862930:
				return "SMBus Controller";
			case 0x808629C0:
				return "Express DRAM Controller";
			case 0x80865910:
				return "Xeon E3-1200 v6/7th Gen Core Processor Host Bridge/DRAM Registers";
			case 0x8086A110:
				return "100 Series/C230 Series Chipset Family PCI Express Root Port #1";
			case 0x8086A12F:
				return "100 Series/C230 Series Chipset Family USB 3.0 xHCI Controller";
			case 0x8086A13A:
				return "100 Series/C230 Series Chipset Family MEI Controller #1";
			case 0x8086A154:
				return "CM238 Chipset LPC/eSPI Controller";
			default:
				return to_hstring(deviceID);
		}
	}

	const char* MassStorageControllerSubclassName(uint8_t subclassCode) {
		switch (subclassCode){
			case 0x00:
				return "SCSI Bus Controller";
			case 0x01:
				return "IDE Controller";
			case 0x02:
				return "Floppy Disk Controller";
			case 0x03:
				return "IPI Bus Controller";
			case 0x04:
				return "RAID Controller";
			case 0x05:
				return "ATA Controller";
			case 0x06:
				return "Serial ATA";
			case 0x07:
				return "Serial Attached SCSI";
			case 0x08:
				return "Non-Volatile Memory Controller";
			case 0x80:
				return "Other";
			default:
				return to_hstring(subclassCode);
		}
	}

	const char* SerialBusControllerSubclassName(uint8_t subclassCode){
		switch (subclassCode){
			case 0x00:
				return "FireWire (IEEE 1394) Controller";
			case 0x01:
				return "ACCESS Bus";
			case 0x02:
				return "SSA";
			case 0x03:
				return "USB Controller";
			case 0x04:
				return "Fibre Channel";
			case 0x05:
				return "SMBus";
			case 0x06:
				return "Infiniband";
			case 0x07:
				return "IPMI Interface";
			case 0x08:
				return "SERCOS Interface (IEC 61491)";
			case 0x09:
				return "CANbus";
			case 0x80:
				return "SerialBusController - Other";
			default:
				return to_hstring(subclassCode);
		}
	}

	const char* BridgeDeviceSubclassName(uint8_t subclassCode){
		switch (subclassCode){
			case 0x00:
				return "Host Bridge";
			case 0x01:
				return "ISA Bridge";
			case 0x02:
				return "EISA Bridge";
			case 0x03:
				return "MCA Bridge";
			case 0x04:
				return "PCI-to-PCI Bridge";
			case 0x05:
				return "PCMCIA Bridge";
			case 0x06:
				return "NuBus Bridge";
			case 0x07:
				return "CardBus Bridge";
			case 0x08:
				return "RACEway Bridge";
			case 0x09:
				return "PCI-to-PCI Bridge";
			case 0x0a:
				return "InfiniBand-to-PCI Host Bridge";
			case 0x80:
				return "Other";
			default:
				return to_hstring(subclassCode);
		}
	}

	const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode) {
		switch (classCode) {
			case 0x01: // mass storage controller
				return MassStorageControllerSubclassName(subclassCode);
			case 0x03:
				switch (subclassCode){
					case 0x00:
						return "VGA Compatible Controller";
					default:
						return to_hstring(subclassCode);
				}
			case 0x06:
				return BridgeDeviceSubclassName(subclassCode);
			case 0x0C:
				return SerialBusControllerSubclassName(subclassCode);
			default:
				return to_hstring(subclassCode);
		}
	}
	const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF) {
		uint32_t IF = (((classCode << 8) | subclassCode) << 8) | progIF;
		switch (IF) {
			case 0x010600:
				return "Vendor Specific Interface";
			case 0x010601:
				return "AHCI 1.0";
			case 0x010602:
				return "Serial Storage Bus";
			case 0x030000:
				return "VGA Controller";
			case 0x030001:
				return "8514-Compatible Controller";
			case 0x0C0300:
				return "UHCI Controller";
			case 0x0C0310:
				return "OHCI Controller";
			case 0x0C0320:
				return "EHCI (USB2) Controller";
			case 0x0C0330:
				return "XHCI (USB3) Controller";
			case 0x0C0380:
				return "Unspecified";
			case 0x0C03FE:
				return "USB Device (Not a Host Controller)";
			default:
				return to_hstring(progIF);
		}
	}
}