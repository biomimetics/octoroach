#!/usr/bin/python
'''
This module requires pygame2 instead of pygame for some reason
I do not remember any more. This is why documentation is important. :D
'''

import pygame2
import sys, os
import time

try:
    import pygame2.sdl.constants as sdlconst
    import pygame2.sdl.event as event
    import pygame2.sdl.video as video    
    import pygame2.sdl.joystick as joystick
except ImportError:
    print ("No pygame2.sdl support")
    sys.exit ()

class Joystick(joystick.Joystick):
    axes = [0, 0, 0, 0]
    hatvalue = 0
    buttons = []
    calibration = False

    def __init__(self, index):
        joystick.init()
        video.init()
        if joystick.num_joysticks() == 0:
            return
        try:
            joystick.Joystick.__init__(self, index)
            print 'joystick found: ' + self.name
        except pygame.error:
            sys.stderr.write('No joysticks found. Exiting.\n')

        for i in range(self.num_buttons):
            self.buttons.append(False)

    def get_buttons(self):
        return self.buttons

    def get_hatvalue(self):
        return self.hatvalue
    
    def get_axes(self):
        return self.axes
        
    def run(self):

        for ev in event.get():
            if ev.type == sdlconst.JOYAXISMOTION and ev.joy == self.index:
                self.axes[ev.axis] = ev.value
            elif ev.type == sdlconst.JOYBUTTONDOWN and ev.joy == self.index:
                self.buttons[ev.button] = True
            elif ev.type == sdlconst.JOYBUTTONUP and ev.joy == self.index:
                self.buttons[ev.button] = False
            elif ev.type == sdlconst.JOYHATMOTION and ev.joy == self.index:
                self.hatvalue = ev.value
            elif ev.type == sdlconst.JOYBALLMOTION and ev.joy == self.index:
                pass

    def get_hat_vertical(self):
        if (self.hatvalue & 1) == 1:
            return 1
        elif (self.hatvalue & 4) == 4:
            return -1
        else:
            return 0

    def get_thrust(self):
        return -int(round(100.0*self.axes[1]/32767))



if __name__ == '__main__':
    js = Joystick(0)
    done = 0
    while (not done):
        try:
            js.run()
            print js.get_thrust()
            print js.get_hat_vertical()
            btns = js.get_buttons()
            print btns
            if btns[0] == True:
                done = 1

            time.sleep(.3)
        except (KeyboardInterrupt, SystemExit):
            done = 1
        

        ''' 
        try:
            r.go()
            print repr(r)
            time.sleep(.3)
        except (KeyboardInterrupt, SystemExit):
            done = 1
        '''    
