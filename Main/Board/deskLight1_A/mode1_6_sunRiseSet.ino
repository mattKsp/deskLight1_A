/*----------------------------mode1 sunrise and mode6 sunset----------------------------*/
/*
 * sunrise and sunset modes (1 & 6) are usually kept out of the preset lists and just triggered by the alarm interrupts
 * ..but it is a mode and can be selected as such 
 * eg. for testing/demo porpoises
 */
void mode1() {
  
  //name should be sunrise!
  //isStatic default is false
  //if triggered by an alarm, then temporarily make sure isStatic is set to false
  if(modeInfo[1].isStatic) {
    //show hyperthetical mid-point levels
  }
  else {
    sunRise();
  }
  
} //END mode1


void mode6() {
  
  //name should be sunset!
  //isStatic default is false
  if(modeInfo[6].isStatic) {
    fadeToBlackBy( leds, _ledNum, 100);
  }
  else {
    fadeToBlackBy( leds, _ledNum, 100);
  }
  
} //END mode6


/*----------------------------set----------------------------*/

void setSunRise(uint8_t hour, uint8_t mins) {
  //mode 0 = natural (set rise to realtime) - Not used yet!
  //mode 1 = set time aswell (mabye link to alarm array entry)
  RTC.disableAlarms();                                                          //hmm.. power cuts? ..and how to specify 1 or both? write my own?
  DateTime MyTimestamp;
  MyTimestamp = RTC.read();                                                       //get an initialized timestamp to use
  MyTimestamp.Hour = hour;
  MyTimestamp.Minute = mins;
  MyTimestamp.Second = 0;
  
  //RTC.setAlarm(DS3231_Simple::ALARM_EVERY_MINUTE); 
  //RTC.setAlarm(MyTimestamp, DS3231_Simple::ALARM_MATCH_MINUTE);
  RTC.setAlarm(MyTimestamp, DS3231_Simple::ALARM_DAILY);                          //sun rise - Alarm 2 - ALARM_DAILY (on the hour and minute)
  
    //RTC.setAlarm(MyTimestamp, DS3231_Simple::ALARM_EVERY_SECOND); 
    //RTC.setAlarm(MyTimestamp, DS3231_Simple::ALARM_MATCH_SECOND); 
    //RTC.setAlarm(MyTimestamp, DS3231_Simple::ALARM_MATCH_SECOND_MINUTE_HOUR);       //sun set - Alarm 1 - ALARM_MATCH_SECOND_MINUTE_HOUR
  
  #ifdef DEBUG
    Serial.println();
    Serial.print(F("void setSunRiseSet (rise/Alarm2) "));
    Serial.print(hour);
    Serial.print(":"); 
    Serial.print(mins);
    Serial.println();
  #endif
}

/*----------------------------do----------------------------*/
/*
 * sunrise/set states can be used by such as static/moving
 * eg. static could go straight to the end colours, or the frozen midpoint of the 'do' state
 * 
 * //setup
 * beginning
 * do rise
 * end
 */
 unsigned long _sunRiseInterval = 0; // the time we need to wait //calculated at runtime
unsigned long _sunRisePreviousMillis = 0; // millis() returns an unsigned long.

void sunRise() {
  //time to rise (ms)
  //start/end levels
  //number of steps (LED total)
  //time interval for each step (ms)
  //levels for each step

  _sunRiseInterval = 10000;
  
  unsigned long _sunRiseCurrentMillis;// = millis(); //empty ok?            //get current time //not really necessary, but didn't want to leave it empty in case..

  if(_sunRiseStateCur == 0) { /* begin */
    
    fill_solid( leds, _ledNum, CRGB(0, 0, 0));  //TEMP - testing
    //fadeToBlackBy( leds, _ledNum, 30);                        //fade to black to setup (sunset will need a fade-up to whatever the starts levels are)
    
//    _sunRiseInterval = 30;    //trying to match 'fade to black by' time
//    _sunRiseCurrentMillis = millis();                         //get current time
//    //wait for the appropriate divison of time
//    if ((unsigned long)(_sunRiseCurrentMillis - _sunRisePreviousMillis) >= _sunRiseInterval) {
//      _sunRisePreviousMillis = millis();                              //save the current time for next round
//    }
    
    //now show first step of sequence
    for(int i=ledSegment[0].first; i < ledSegment[0].last; i++) {
      leds[i] = CRGB(10, 10, 10); //???
    }
    
    _sunRiseCurrentMillis = millis();                         //get current time
    //wait for the appropriate divison of time
    if ((unsigned long)(_sunRiseCurrentMillis - _sunRisePreviousMillis) >= _sunRiseInterval) {
      _sunRiseStateCur = 1;                                   //set next state
      _sunRisePreviousMillis = millis();                              //save the current time for next round
    }
  }
  else if(_sunRiseStateCur == 1) { /* do */
    _sunRiseCurrentMillis = millis();                         //get current time
    //do/show rest of sequence (except the last step)
    
    fill_gradient_RGB(leds, ledSegment[1].first, CRGB(255, 255, 255), ledSegment[1].last, CRGB(0, 0, 0));
    fill_gradient_RGB(leds, ledSegment[3].first, CRGB(0, 0, 0), ledSegment[3].last, CRGB(255, 255, 255) );
    
    //when finished..
    if ((unsigned long)(_sunRiseCurrentMillis - _sunRisePreviousMillis) >= _sunRiseInterval) {
      _sunRiseStateCur = 2; //set next state
      _sunRisePreviousMillis = millis();                                 //save the current time for next round
    }
  }
  else if(_sunRiseStateCur == 2) { /* end */
    _sunRiseCurrentMillis = millis();                         //get current time
    
    //show last step of sequence
    for(int i=ledSegment[2].first; i<ledSegment[2].last; i++) {
      leds[i] = CRGB(32, 32, 255);
    }
    
    //wait for the appropriate divison of time
    if ((unsigned long)(_sunRiseCurrentMillis - _sunRisePreviousMillis) >= _sunRiseInterval) {
      _modePresetSlotCur += 1;
      if(_modePresetSlotCur >= _modePresetSlotNum){ _modePresetSlotCur = 0; }  //TEMP rollover catch
      _modeCur = _modePreset[_modePresetSlotCur];
      //_modePresetSlotCur += 1;                                  //TEMP - next mode
      _sunRiseStateCur = 0;                                     //reset for next time
      _sunRisePreviousMillis = millis();                                 //save the current time for next round
    }
  } //END else if
/*
    int startPx0 = (0 - _segmentTotal);
    int endPx0 = 0;
    //map(int hour, 0, 12, _segmentStart[0], _segmentEnd[_segmentTotal-1]);
    
    for(int i=ledSegment[0].first; i<ledSegment[0].last; i++) {
      leds[i] = CRGB(255, 255, 255);
    }
    for(int i=ledSegment[2].first; i<ledSegment[2].last; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    
    fill_gradient_RGB(leds, ledSegment[1].first, CRGB(255, 255, 255), ledSegment[1].last, CRGB(0, 0, 0));
    fill_gradient_RGB(leds, ledSegment[3].first, CRGB(0, 0, 0), ledSegment[3].last, CRGB(255, 255, 255) );

    //when sunrise is finished either stay bright, or change(fade) to previous levels
    //set tomorrows sunrise time
    //_sunRiseEnabled = false;                //reset
    //set next mode as morning or day
*/
  
} //END sunRise

