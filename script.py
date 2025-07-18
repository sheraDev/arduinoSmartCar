from os import system as s
from sys import exit
import questionary
import bluetooth
import keyboard
from time import sleep

socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect(("00:21:13:00:32:E0", 1))


def on_key_event(e):
    if e.event_type == keyboard.KEY_DOWN:
        pressed = e.name
        
        if pressed in ["up", "down", "right", "left", "space"]:

            if(pressed == "up"):
                socket.send("F")
            elif(pressed == "down"):
                socket.send("B")
            elif(pressed == "right"):
                socket.send("R")
            elif(pressed == "left"):
                socket.send("L")
            elif(pressed == "space"):
                socket.send("S")


def non_auto_modes(socket, mode):
    socket.send(mode)

    # Ajoute un gestionnaire d'événements pour toutes les touches du clavier
    keyboard.hook(on_key_event)

    # Maintient le script en cours d'exécution
    keyboard.wait("esc")
    s("clear")
    keyboard.unhook_all()
    
def exit_all(socket):
    socket.send("Y")
    sleep(0.2)
    socket.send("S")
       

def main():

    while socket:
        
        questions = [{
                'type': 'select',
                'name': 'mode',
                'message': 'Choisissez le mode :',
                'choices': [
                    {'name': 'Auto mode', 'value': 'X'},
                    {'name': 'Bluetooth mode', 'value': 'Y'},
                    {'name': 'Bluetooth with control', 'value': 'Z'},
                    {'name': 'Shutdown all modes', 'value': 'Q'},
                    {'name': 'Exit', 'value': 'EXIT'},

                ],
            },
        ]

        answers = questionary.prompt(questions)

        # answers["mode"] sera par X, Y, Z ou 0
        
        if answers["mode"] == "Q":
           exit_all(socket)


        elif answers["mode"] == "EXIT":
            exit_all(socket)
            socket.close()
            exit()

        else:
            if answers["mode"] == "X":
                socket.send('X')
            else:
                non_auto_modes(socket, answers["mode"])



if __name__ == "__main__":
    main()
        