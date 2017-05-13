class Crc16_Calc:
    def __init__(self, init=0xFFFF, mask=0x8408):
        """
        Creates an object capable of calculating the crc of a list
        using the initial value 'init' and the 'mask', this data
        should be given once the object is created otherwise the default values
        will be used. You have two methods to calculate the 16 bit CRC, one
        using bit-a-bit calculation and the other is done with an look-up table
        (for faster calculation). The first time the calculation is done, the
        table will be created
        """
        self.init = init
        self.mask = mask
        self.table = []
        self.gentable()

    def bitcrc(self, *args):
        crc = self.init
        for i in args:
            crc = crc ^ i
            for j in range(8):
                if (crc & 1):
                    crc = (crc >> 1) ^ self.mask
                else:
                    crc >>= 1
                i >>= 1
        return crc

    def gentable(self):
        for i in range(256):
            crc = i
            for j in range(8):
                if (crc & 1):
                    crc = self.mask ^ (crc >> 1)
                else:
                    crc >>= 1
            self.table.append(crc)

    def tablecrc(self, *args):
        if (len(self.table) == 0):
            self.gentable()
        crc = self.init
        for i in args:
            crc = (crc >> 8) ^ self.table[((crc ^ i) & 0xFF)]
        return crc

if __name__ == "__main__":
    a = Crc16_Calc(0xFFFF, 0xa6bc)
    data = [0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19]
    result = a.tablecrc(*data)
    print("Result of crc calculation using table"
          " is 0x{:04X}".format(result))
