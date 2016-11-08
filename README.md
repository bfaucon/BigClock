# BigClock
Big LED Clock Project

Starting building a big scoreboard with Arduino.

Clock Project: 
http://www.instructables.com/id/Big-auto-dim-room-clock-using-arduino-and-WS2811/
https://github.com/saxos1983/Arduino/tree/master/DigitClock
https://github.com/abdelhadj/Arduino_Projects/blob/master/Big7SegmentClockIR

Scoreboard project:
http://www.instructables.com/id/Remotely-Controlled-Multifunction-Scoreboard-Based/
http://www.instructables.com/id/Remote-Controlled-Arduino-Scoreboard-using-LED-Str/

CountDown timer Project:
https://sites.google.com/a/stu.dsd2.org/thetreyclock/home/code
http://pastebin.com/f57045830

How it Works
- Sending key to monitor:
1:  Change mode 1) Clock, 2) Match, 3) Temp
2:  Stop/Start Timer
3:  Reset Timer to 35:00 
4: Hour +1, Score L +1
5: Hour -1, Score L -1
6: Min +1, Score V +1
7: Min -1, Score V -1
8: tbd


Materials:
- Arduino Ok
- RTC Ok
- LED ordered

Phases:
- Develop Clock using monitor
- Develop ScoreBoard logic using monitor
- Adapt to LED
- swith timer 35 25 17.30 10 5  (tbd??)
- Adding button (tbd)


Possible futur development:
- Adding Remote with 433Mhz https://www.carnetdumaker.net/articles/communiquer-sans-fil-en-433mhz-avec-la-bibliotheque-virtualwire-et-une-carte-arduino-genuino/
- Adding temp
- Adding Wifi to access current time and score from internet.
- Adding Horn
