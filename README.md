# Car-Black-Box

The Car Black Box is an embedded system designed to monitor and log critical vehicular events, similar to aviation black boxes. It captures data such as speed, gear shifts, and collisions, and allows users to interact via a  menu screen. Users can download the stored logs to a laptop using UART communication. The project is implemented on a PIC microcontroller with external EEPROM for event logging.<br />

Operations<br />

1.View Log<br />

*Displays the last 10 events stored in the external EEPROM.<br />
*Users can scroll through the events.<br /><br />

2.Clear Log<br />

*Clears all stored events from EEPROM.<br />
*After clearing, no events will be visible in the View Log.<br />
*Scrolling becomes ineffective after clearing.<br /><br />

3.Download Log<br />

*Connects to a PC via UART communication.<br />
*Long-pressing the Download Log option prints the stored data on the Serial Monitor.<br /><br />

4.Set Time<br />

*Allows the user to set time for the RTC module to maintain accurate event timestamps.<br /><br />


