STX = chr(2)			    # start transmission marker
ETX = chr(3)		        # end transmission marker
HEADER_LENGTH = 4	        # number of bytes required for packet header
TRAILER_LENGTH = 2          # number of bytes required for packet trailer
STATUS_OFFSET = 1	        # number of bytes from STX to STATUS
TYPE_OFFSET = 2	            # number of bytes from STX to TYPE
LENGTH_OFFSET = 3	        # number of bytes from STX to LENGTH
DATA_OFFSET = 4	            # number of bytes from STX to the data
CHECKSUM_OFFSET = 4	        # number of bytes from STX+[length] to CHECKSUM
NOETXSTXCHECKSUM = 3	    # number of bytes used by STX,ETX,CHECKSUM


class StxEtx():

    """
    A class for packetizing serial communications.

    Methods:
        set_data_out
        send
        process
        get_rx_packet_status
        get_rx_packet_type
        get_rx_packet_data_length
        get_rx_packet_data

    Members:
        data_out
        rx_packet

    """

    def __init__(self):
        self.data_out = None
        self.rx_packet = []

    def set_data_out(self, func):
        """
        Set the function for sending packet data.

        Set the output function for the packet data. This
        is typically something like the write() method of
        a serial connection.
        """

        self.data_out = func

    def send(self, status, packet_type, payloaddata):
        """
        Create packet structure and send payloaddata.

        Create a packet from payload data, and integer 
        status and packet_type indicators.
        """
        checksum = status + packet_type + len(payloaddata)
        #print payloaddata
        for datum in payloaddata:
            checksum += int(ord(datum))
        checksum = chr(checksum & int('11111111', 2))
        dout = STX + chr(status) + chr(packet_type) + \
            chr(len(payloaddata)) + ''.join(payloaddata) + checksum + ETX
        self.data_out(dout)

    def process(self, rx_buffer):
        """
        Process a receiving buffer and look for packets.

        Process the receiving buffer and return a boolean
        indicating whether a packet was found. Also, returns
        the next index of the buffer at which to begin looking
        for the next packet.
        """
        foundpacket = False
        next_idx = -1
        for idx in range(len(rx_buffer)):
            element = rx_buffer[idx]
            # Check to see if we have the beginning of a packet
            if element == STX and len(rx_buffer) >= (idx + LENGTH_OFFSET):
                length = int(ord(rx_buffer[idx + LENGTH_OFFSET]))
                # Make sure the rx_buffer has enough data in it to contain the
                # entire packet
                if (len(rx_buffer) - idx >= HEADER_LENGTH +
                    length + TRAILER_LENGTH):
                    # Is the end transmission byte where we expect it to be?
                    if(rx_buffer[idx + HEADER_LENGTH + length +
                       TRAILER_LENGTH - 1] == ETX):

                        tmp_buffer = [ord(item) for item in rx_buffer]
                        checksum = sum(tmp_buffer[idx + 1:idx +
                                       HEADER_LENGTH + length])
                        checksum = chr(checksum & int('11111111', 2))
                        # Do the checksums match? If so, congratulations! You
                        # have a packet. Isn't that exciting?
                        if checksum == rx_buffer[idx + CHECKSUM_OFFSET +
                                              length]:

                            self.rx_packet = rx_buffer[idx + 1:idx +
                                                  HEADER_LENGTH + length]
                            foundpacket = True
                            # Don't forget to set the next index to start
                            # looking for another packet all over again.
                            next_idx = idx + HEADER_LENGTH + length + \
                                      TRAILER_LENGTH
                            break
                        else:
                            self.rx_packet = []
                else:
                    break

        return foundpacket, next_idx

    def get_rx_packet_status(self):
        """Return the integer status of a received packet."""
        if len(self.rx_packet) > (STATUS_OFFSET - 1):
            return ord(self.rx_packet[STATUS_OFFSET - 1])
        else:
            return -1

    def get_rx_packet_type(self):
        """Return the integer packet type of a received packet."""
        if len(self.rx_packet) > (TYPE_OFFSET - 1):
            return ord(self.rx_packet[TYPE_OFFSET - 1])
        else:
            return -1

    def get_rx_packet_data_length(self):
        """Return the length of the payload data of a received packet."""
        if len(self.rx_packet) > (LENGTH_OFFSET - 1):
            return ord(self.rx_packet[LENGTH_OFFSET - 1])
        else:
            return -1

    def get_rx_packet_data(self):
        """Return the received packet's payload data."""
        if len(self.rx_packet) > (LENGTH_OFFSET):
            return self.rx_packet[HEADER_LENGTH -
                                       1:len(self.rx_packet)]
        else:
            return []
