function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  switch (bytes[0]) {
    case 0x01:
      decoded.tipo = "amps";
      decoded.amp1 = bytes[1];
      decoded.amp2 = bytes[2];
      decoded.amp3 = bytes[3];
      decoded.amp4 = bytes[4];
      break;
    case 0x02:
      decoded.tipo = "caida";
      decoded.caida = bytes[1];
      break;
    case 0x04:
      decoded.tipo = "gotas";
      decoded.triggs = (bytes[1] << 8)    + bytes[2];
      if (decoded.triggs !== 0) {
        decoded.gotas = 1;
      } else {
        decoded.gotas = 0;
      }
      break;
    case 0x08:
      decoded.tipo = "temphum";
      decoded.temp = (bytes[1] << 8)    + bytes[2];
      decoded.hum = (bytes[3] << 8)    + bytes[4];

      break;
    case 0x16:
      decoded.tipo = "puerta";
      decoded.triggs = (bytes[1] << 8)    + bytes[2];
      break;
  }
  return decoded;
}
