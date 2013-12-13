class Device {
  /*
   * Interface class with purely virtual methods
   */
 public:
  virtual String getMacAddress();
  virtual InetAddress getIP();
  virtual DeviceType getDeviceType();
  virtual int getProtocolVersion();
  virtual int getVendorId();
  virtual int getProductId();
  virtual int getHardwareRevision();
  virtual int getSoftwareRevision();
  virtual long getLinkSpeed();
}
