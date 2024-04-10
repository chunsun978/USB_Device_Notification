#include <libudev.h>
#include <iostream>
#include <functional>
#include <poll.h>

// libudev is a library for managing devices, and it is used to monitor USB devices in this case.
// and it is a C library, so we need to wrap it in a C++ class.

//example of using udev to monitor USB devices.
#if 0  ====================================
struct udev *udev;
struct udev_device *dev;

udev = udev_new();
if (!udev) {
    printf("Can't create udev\n");
    exit(1);
}

// Get a handle to the device 
dev = udev_device_new_from_syspath(udev, "/sys/class/input/event0");

// Access device properties 
const char *path = udev_device_get_devnode(dev);
const char *subsystem = udev_device_get_subsystem(dev);

printf("Device path: %s\n", path);
printf("Device subsystem: %s\n", subsystem);

// Clean up 
udev_device_unref(dev);
udev_unref(udev);
============================================
#endif

namespace usb {
class Device {
    struct udev_device *dev;
	
    public:
	Device() = delete;
	Device(struct udev_device* dev) : dev(dev) {}

	Device(const Device& other) : dev(other.dev) {
	    udev_device_ref(dev);
	}

	~Device() {
            udev_device_unref(dev);
	}

	std::string action() const { return udev_device_get_action(dev); }

	std::string attr(const char* name) const {
	    const char* val = udev_device_get_sysattr_value(dev, name);
	    return val ? val : "";
	}
};

class Monitor {
    struct udev_monitor *mon;
     public:
	Monitor() {
    	struct udev* udev = udev_new();
        mon = udev_monitor_new_from_netlink(udev, "udev");
			udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
			udev_monitor_enable_receiving(mon);
	}

	Monitor(const Monitor& other) = delete;

	~Monitor() {
	    udev_monitor_unref(mon);
	}

	Device wait(std::function<bool(const Device&)> process) {
	    struct pollfd fds[1];
	    fds[0].events = POLLIN;
	    fds[0].fd = udev_monitor_get_fd(mon);

	    while (true) {
	        int ret = poll(fds, 1, -1);
	        if (ret < 0) {
		    throw std::system_error(errno, std::system_category(),
		        "Poll failed");
		}
		if (ret) {
		    Device d(udev_monitor_receive_device(mon));
		    // call process() to check if the device is matched.
		    // In this case, it will check if the USB ID is matched.
		    if (process(d)) {
		        return d;
		    };
		}
	    }
      }
};
};

int main() {
    usb::Monitor mon;
    usb::Device d = mon.wait([](auto& d) { 
	// this is a lambda function to check if the USB ID is matched.
	// if matched, it will exit and display pwer usage.
	// if not, the loop will continue.
	auto id = d.attr("idVendor") + ":" + d.attr("idProduct");
	auto produce = d.attr("product");
	std::cout << "Check [" << id << "] action: " << d.action() << std::endl;
	return d.action() == "bind" && id == "1f75:0918";
    });
    std::cout << d.attr("product") << " connected, uses up to "
				<< d.attr("bMaxPower") << std::endl;
    return 0;
}
