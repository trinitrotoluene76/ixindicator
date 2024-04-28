# script côté PC qui trouve le dongle T-display S3 sur la liaison série grace au VID et PID
# robuste à l'absence de dongle ou arrachage USB
# envoi un message au dongle à 1Hz

import serial.tools.list_ports
import serial
import time

def find_t_dongle_port():
    ports = serial.tools.list_ports.comports()
    for port, desc, hwid in sorted(ports):
        if "VID:PID=303A:1001" in hwid:  # Vérifie si le dongle est connecté
            print("Port trouvé")
            return port
    return None

def main():
    
    t_dongle_port = find_t_dongle_port()
    if t_dongle_port is None:
        print("Aucun T-Dongle-S3 trouvé.")
        exit()

    print(f"T-Dongle-S3 trouvé sur le port {t_dongle_port}.")

    try:
        # Ouvre la connexion série
        ser = serial.Serial(t_dongle_port, 115200, timeout=1)
        ser.flush()
        
        time.sleep(1)  
        print(f"Script de démo.")

        while True:
            try:
                # mode 4: affiche "passed" sans chrono
                ser.write(b"$PASS\n")
                ser.flush()
                time.sleep(3)  

                # mode 5: affiche "failed" sans chrono
                ser.write(b"$FAIL\n")
                ser.flush()
                time.sleep(3)  

                # mode 1 affichage d'un simple chrono
                # fonction d'affichage du temps sous différents format
                # hh:mm:ss
                # dd jours hh:mm
                # > 49 jours
                ser.write(b"$START\n")
                ser.flush()
                time.sleep(3)  

                # mode 4: affiche "passed" avec chrono à 3s
                ser.write(b"$PASS\n")
                ser.flush()
                time.sleep(3)
                
                # mode 5: affiche "failed" avec chrono à 6s
                ser.write(b"$FAIL\n")
                ser.flush()
                time.sleep(3)

                # mode 2 (watchdog latché), affiche une progress barre puis "refresh time>x s" sur fond rouge
                ser.write(b"$START,2,3\n") # mode 2, timeout [1s;2^32-1s] 3s
                ser.flush()
                time.sleep(2)  # ok

                ser.write(b"$START,2,3\n") # mode 2, timeout [1s;2^32-1s] 3s
                ser.flush()
                time.sleep(4)  # timeout

                ser.write(b"$STOP\n") # Affiche "waiting command"
                ser.flush()
                time.sleep(3)  
                
                # mode 3 watchdog non latché: affiche une progress barre et "nb timeouts:xx" sur fond noir
                # nécessite de rafraichir la barre avec de nouvelles commandes START, sinon on reste dans le mode.
                # Pour sortir du mode, il faut débrancher ou envoyer une commande STOP
                ser.write(b"$START,3,2\n") # mode 3, timeout [1s;2^32-1s] 2s
                ser.flush()
                time.sleep(1)  # ok

                ser.write(b"$START,3,2\n") # mode 3, timeout [1s;2^32-1s] 2s
                ser.flush()
                time.sleep(4)  # timeout

                ser.write(b"$START,3,2\n") # mode 3, timeout [1s;2^32-1s] 2s
                ser.flush()
                time.sleep(3)  # timeout

                ser.write(b"$START,3,2\n") # mode 3, timeout [1s;2^32-1s] 2s
                ser.flush()
                time.sleep(1)  # ok

                ser.write(b"$STOP\n") # Affiche "waiting command" (On est obligé de cloturer par un stop pour éviter d'avoir une prochaine commande avec startime>0)
                ser.flush()
                time.sleep(1)

            except serial.SerialException:
                print("Déconnexion du T-Dongle-S3.")
                break  # Sortir de la boucle interne et rechercher à nouveau le port

    except serial.SerialException:
        print("Échec de la connexion au T-Dongle-S3.")
        exit()

if __name__ == "__main__":
    main()
