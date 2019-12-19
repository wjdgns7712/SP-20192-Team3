import RPi.GPIO as GPIO
import pyautogui

GPIO.setmode(GPIO.BCM)
GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(19, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(16, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(26, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(20, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def my_callback_w(channel):
    pyautogui.press('w')

def my_callback_s(channel):
    pyautogui.press('s')

def my_callback_a(channel):
    pyautogui.press('a')
 
def my_callback_d(channel):
    pyautogui.press('d')

GPIO.add_event_detect(19, GPIO.FALLING, callback=my_callback_w)
GPIO.add_event_detect(26, GPIO.FALLING, callback=my_callback_s)
GPIO.add_event_detect(20, GPIO.FALLING, callback=my_callback_a)
GPIO.add_event_detect(16, GPIO.FALLING, callback=my_callback_d)

try:
    print("Waiting f or falling edge on port 21")
    GPIO.wait_for_edge(21, GPIO.RISING)
    print("Falling edge detected. Here endeth the second lesson.")

except KeyboardInterrupt:
    GPIO.cleanup()       # clean up GPIO on CTRL+C exit
    
GPIO.cleanup()           # clean up GPIO on normal exit
