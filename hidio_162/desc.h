#include "util.h"
#define VID              0x03eb
#define PID              0x2013
#define STR_MANUFACTURER L"DSKK"
#define STR_PRODUCT      L"HIDIO_SAMPLE"
#define DEVICE_DESC_SIZE 18
#define CONFIG_DESC_SIZE 41
#define REPORT_DESC_SIZE 53
#define NUM_EP           2

#define EP_TYPE_CONTROL         0x00
#define EP_TYPE_INTERRUPT_IN    0xC1
#define EP_TYPE_INTERRUPT_OUT   0xC0

#define EP_SINGLE_BUFFER        0x02
#define EP_DOUBLE_BUFFER        0x06

#define EP_SIZE(s) ((s) == 64 ? 0x30 :\
                   ((s) == 32 ? 0x20 :\
                   ((s) == 16 ? 0x10 :\
                                0x00)))

struct ep_list_struct {
    uint8_t data_transfer_type;
    uint8_t ep_size_buffer_type; // =ep_size|buffer_type
    // data_transfer_type and buffer_type is defined in init.h
};

struct desc_list_struct {
    uint16_t       wValue; // MSB=descriptor type (see memo) LSB=descriptor index
    uint16_t       wIndex;
    const uint8_t* addr;
    uint8_t        length;
};

struct str_desc_struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    int16_t wString[];
};

const uint8_t device_desc[DEVICE_DESC_SIZE] PROGMEM = {
    18,                // bLength
    1,                 // bDescriptorType
    0x00, 0x02,        // bcdUSB
    0,                 // bDeviceClass
    0,                 // bDeviceSubClass
    0,                 // bDeviceProtocol
    64,                // bMaxPacketSize0
    LSB(VID),          // idVendor[0]
    MSB(VID),          // idVendor[1]
    LSB(PID),          // idProduct[0]
    MSB(PID),          // idProduct[1]
    0x00, 0x10,        // bcdDevice (=version number)
    1,                 // iManufacturer
    2,                 // iProduct
    0,                 // iSerialNumber
    1                  // bNumConfigurations (this source does not support composite device)
};

const uint8_t config_desc[CONFIG_DESC_SIZE] PROGMEM = {
    // configuration descriptor
    9,                              // bLength;
    2,                              // bDescriptorType;
    LSB(CONFIG_DESC_SIZE),
    MSB(CONFIG_DESC_SIZE),          // wTotalLength
    1,                              // bNumInterfaces
    1,                              // bConfigurationValue
    0,                              // iConfiguration
    0x80,                           // bmAttributes
    50,                             // bMaxPower
    // interface descriptor
    9,                              // bLength
    4,                              // bDescriptorType
    0,                              // bInterfaceNumber
    0,                              // bAlternateSetting
    NUM_EP,                         // bNumEndpoints (is 2 if the device has INTR_IN)
    0x03,                           // bInterfaceClass (0x03 = HID)
    0x00,                           // bInterfaceSubClass (0x00 = No Boot)
    0x00,                           // bInterfaceProtocol (0x00 = No Protocol)
    0,                              // iInterface
    // HID interface descriptor
    9,                              // bLength
    0x21,                           // bDescriptorType
    0x11, 0x01,                     // bcdHID
    0,                              // bCountryCode
    1,                              // bNumDescriptors
    0x22,                           // bDescriptorType
    LSB(REPORT_DESC_SIZE),
    MSB(REPORT_DESC_SIZE),          // wDescriptorLength
    // endpoint descriptor
    // EP1
    7,                              // bLength
    5,                              // bDescriptorType
    1 | 0x80,                       // bEndpointAddress (|0x80=in)
    0x03,                           // bmAttributes (0x03=intr)
    64, 0,                          // wMaxPacketSize
    4,                              // bInterval
    // EP2
    7,                              // bLength
    5,                              // bDescriptorType
    2,                              // bEndpointAddress (|0x80=in)
    0x03,                           // bmAttributes (0x03=intr)
    64, 0,                          // wMaxPacketSize
    4                               // bInterval
    // add some descriptors if needed
};

const uint8_t report_desc[REPORT_DESC_SIZE] PROGMEM = {
    0x06, 0xFF, 0xFF,  //  Usage Page (vendordefined?)
    0x09, 0x01,        //  Usage      (vendordefined
    0xA1, 0x01,        //  Collection (Application)
    // IN report
    0x09, 0x02,        //  Usage      (vendordefined)
    0x09, 0x03,        //  Usage      (vendordefined)
    0x15, 0x00,        //  Logical Minimum(0 for signed byte?)
    0x26 ,0xFF, 0x00,  //  Logical Maximum(255 for signed byte?)
    0x75, 0x08,        //  Report Size(8) = field size in bits = 1 byte
    0x95, 0x08,        //  ReportCount(size) = repeat count of previous item
    0x81, 0x02,        //  IN report (Data,Variable, Absolute)
    // OUT report
    0x09, 0x04,        //  Usage      (vendordefined)
    0x09, 0x05,        //  Usage      (vendordefined)
    0x15, 0x00,        //  Logical Minimum(0 for signed byte?)
    0x26, 0xFF, 0x00,  //  Logical Maximum(255 for signed byte?)
    0x75, 0x08,        //  Report Size(8) = field size in bits = 1 byte
    0x95, 0x08,        //  ReportCount(size) = repeat count of previous item
    0x91, 0x02,        //  OUT report (Data,Variable, Absolute)
    // Feature report
    0x09, 0x06,        //  Usage      (vendordefined)
    0x09, 0x07,        //  Usage      (vendordefined)
    0x15, 0x00,        //  LogicalMinimum(0 for signed byte)
    0x26, 0xFF, 0x00,  //  Logical Maximum(255 for signed byte)
    0x75, 0x08,        //  Report Size(8) =field size in bits = 1 byte
    0x95, 0x04,        //  ReportCount
    0xB1, 0x02,        //  Feature report
    0xC0               //  End Collection
};

const struct str_desc_struct str_desc_langID PROGMEM = {
    4,
    3,
    {0x0409}
};

//string descriptor 1 = manufacturer
const struct str_desc_struct str_desc_manufacturer PROGMEM = {
    sizeof(STR_MANUFACTURER),
    3,
    STR_MANUFACTURER
};

const struct str_desc_struct str_desc_product PROGMEM = {
    sizeof(STR_PRODUCT),
    3,
    STR_PRODUCT
};

#define DESC_LIST_LEN 7
const struct desc_list_struct desc_list[DESC_LIST_LEN] PROGMEM = {
    {0x0100, 0x0000, (const uint8_t*)device_desc,                  DEVICE_DESC_SIZE        },
    {0x0200, 0x0000, (const uint8_t*)config_desc,                  CONFIG_DESC_SIZE        },
    {0x2100, 0x0000, (const uint8_t*)config_desc+DEVICE_DESC_SIZE, 9                       },
    {0x2200, 0x0000, (const uint8_t*)report_desc,                  REPORT_DESC_SIZE        },
    {0x0300, 0x0000, (const uint8_t*)&str_desc_langID,             4                       },
    {0x0301, 0x0409, (const uint8_t*)&str_desc_manufacturer,       sizeof(STR_MANUFACTURER)},
    {0x0302, 0x0409, (const uint8_t*)&str_desc_product,            sizeof(STR_PRODUCT)     }
};

#define EP_LIST_LEN 3
const struct ep_list_struct ep_list[EP_LIST_LEN] PROGMEM = {
    {EP_TYPE_CONTROL,   EP_SIZE(64)|EP_SINGLE_BUFFER},
    {EP_TYPE_INTERRUPT_IN, EP_SIZE(16)|EP_SINGLE_BUFFER},
    {EP_TYPE_INTERRUPT_OUT, EP_SIZE(16)|EP_SINGLE_BUFFER}
};
