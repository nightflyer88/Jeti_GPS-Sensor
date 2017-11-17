/*
  -----------------------------------------------------------
            Menu handler for Jetibox
  -----------------------------------------------------------
*/


// Jetibox screens, names and order
enum screenViews {
  aboutScreen,
  resetAltitude,
  setGpsMode,
  setDistanceMode,
  detectedPressureSensor,
  setFilterX,
  setFilterY,
  setDeadzone,
  setAnalog1,
  setAnalog2,
  setAnalog3,
  setAnalog4,
  setUnits,
  saveSettings,
  defaultSettings
};


void HandleMenu()
{ 
  static int  _nMenu = aboutScreen;
  static char _buffer[17];
  static bool _bSetDisplay = true;
  static uint32_t LastKey;
  uint8_t c = jetiEx.GetJetiboxKey();

  enum
  {
    keyRight       = 0xe0,
    keyLeft        = 0x70,
    keyUp          = 0xd0,
    keyDown        = 0xb0,
    keyUpDown      = 0x90,
    keyLeftRight   = 0x60
  };

  if( c == 0 && !_bSetDisplay) return;

  if( millis() < LastKey ) 
    return; 
  LastKey = millis() + 200; 

  startHandleMenu:
  
  // Right
  if ( c == keyRight && _nMenu < defaultSettings)
  {
    _nMenu++;
    _bSetDisplay = true;
  }

  // Left
  if ( c == keyLeft &&  _nMenu > aboutScreen )
  {
    _nMenu--;
    _bSetDisplay = true;
  }

  // UP
  if ( c == keyUp )
  {
    switch ( _nMenu )
    {
      case setFilterX:
        if (pressureSensor.filterX < 0.98) {
          pressureSensor.filterX += 0.01;
        }
        break;
      case setFilterY:
        if (pressureSensor.filterY < 0.98) {
          pressureSensor.filterY += 0.01;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone < 100) {
          pressureSensor.deadzone++;
        }
        break;
    }
    _bSetDisplay = true;
  }
  
  // DN
  if ( c == keyDown )
  {
    switch ( _nMenu )
    {
      case resetAltitude:
        resetFunc();
      case setUnits:
        if(units == EU){
          units = US;
        }else{
          units = EU;
        }
        break;
      case setGpsMode:
        gpsSettings.mode++;
        if(gpsSettings.mode > GPS_extended){
          gpsSettings.mode = GPS_disabled;
        }
        break;
      case setDistanceMode:
        gpsSettings.distance3D = !gpsSettings.distance3D;
        break;
      case setFilterX:
        if (pressureSensor.filterX > 0.01) {
          pressureSensor.filterX -= 0.01;
        }
        break;
      case setFilterY:
        if (pressureSensor.filterY > 0.01) {
          pressureSensor.filterY -= 0.01;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone > 0) {
          pressureSensor.deadzone--;
        }
        break;
      case saveSettings:
        EEPROM.write(0, units);
        EEPROM.write(1, gpsSettings.mode);
        EEPROM.write(2, gpsSettings.distance3D);
        for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
          EEPROM.write(3+i, analogInputMode[i]);
        }
        EEPROM.write(10,int(pressureSensor.filterX*100));
        EEPROM.write(11,int(pressureSensor.filterY*100));
        EEPROM.write(12,pressureSensor.deadzone);
        resetFunc();
      case defaultSettings:
        for(int i=0; i < 20; i++){
          EEPROM.write(i, 0xFF);
        }
        resetFunc();
    }
    if(_nMenu >= setAnalog1 && _nMenu <= setAnalog4){
      analogInputMode[_nMenu-setAnalog1] = !analogInputMode[_nMenu-setAnalog1];
    }
  
    _bSetDisplay = true;
  }

  if ( !_bSetDisplay )
    return;
 
  switch ( _nMenu )
  {
    case aboutScreen:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "VarioGPS Sensor" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, VARIOGPS_VERSION);
      break;
    case resetAltitude:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Reset zero point" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Press: Down" );
      break;
    case setUnits:
      if(units == EU){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Units: \x45\x55\x28\x6D\x2C\xDF\x43\x29" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Units: \x55\x53\x28\x66\x74\x2C\xDF\x46\x29" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setGpsMode:
      switch (gpsSettings.mode){
        case GPS_disabled:
          jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Disabled" );
          break;
        case GPS_basic:
          jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Basic" );
          break;
        case GPS_extended:
          jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Extended" );
          break;
      }  
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setDistanceMode:
      if(gpsSettings.mode == GPS_disabled)goto startHandleMenu;
      if(gpsSettings.distance3D){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS distance: 3D" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS distance: 2D" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case detectedPressureSensor:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Pressure Sensor:" );
      switch (pressureSensor.type) {
        case unknown:
            jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Not Detected!" );
            break;
        case BMP280:
            jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Found BMP280" );
            break;
        case BME280:
            jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Found BME280" );
            break;
        case MS5611:
            jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Found MS5611" );
            break;
        case LPS:
            jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Found LPS" );
            break;
      }
      break;
    case setFilterX:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _buffer, "Filter X: 0.%2d",int(pressureSensor.filterX*100));
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, _buffer );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Up/Down" );
      break;
    case setFilterY:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _buffer, "Filter Y: 0.%2d",int(pressureSensor.filterY*100));
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, _buffer );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Up/Down" );
      break;
    case setDeadzone:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _buffer, "Deadzone: %2d",pressureSensor.deadzone);
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, _buffer );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Up/Down" );
      break;
    case saveSettings:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Save and Restart" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Save: Down" );
      break;
    case defaultSettings:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Reset Defaults" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Reset: Down" );
      break;
  }
  if(_nMenu >= setAnalog1 && _nMenu <= setAnalog4){
    //sprintf( _buffer, "Volt%c:", analogInCount );
    strcpy( _buffer, "Volt : ");
    _buffer[4] = _nMenu-setAnalog1+49;
    if(analogInputMode[_nMenu-setAnalog1] == analog_enabled){
      strcpy( _buffer + strlen(_buffer), "Enabled");
    }else{
      strcpy( _buffer + strlen(_buffer), "Disabled");
    }  
    jetiEx.SetJetiboxText( JetiExProtocol::LINE1, _buffer );
    jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
  }
  _bSetDisplay = false;
}
