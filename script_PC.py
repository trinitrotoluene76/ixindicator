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
        
        time.sleep(1)  # Attends une seconde avant d'envoyer le prochain message

        while True:
            try:
                # Envoie un message au T-Dongle-S3
                ser.write(b"$START\n") # mode 1 (affiche in progress et fait défiler un chrono)
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

                ser.write(b"$PASS\n") # arrete le chrono et affiche "passed"
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

                ser.write(b"$FAIL\n") # arrete le chrono et affiche "failed"
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

                ser.write(b"$STOP\n") # Affiche "waiting command"
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

                ser.write(b"$START,2,10\n") # mode 2 (watchdog latché), timeout 10s
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

                ser.write(b"$START,3,10\n") # mode 3 (watchdog non latché), timeout 10s
                ser.flush()
                time.sleep(3)  # Attends une seconde avant d'envoyer le prochain message

            except serial.SerialException:
                print("Déconnexion du T-Dongle-S3.")
                break  # Sortir de la boucle interne et rechercher à nouveau le port

    except serial.SerialException:
        print("Échec de la connexion au T-Dongle-S3.")
        exit()

if __name__ == "__main__":
    main()
