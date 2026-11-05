# prueba_escritura.txt
with open("prueba_fija.txt", "w", encoding="utf-8") as f:
    # Cabecera de columnas
    f.write("Muestra;Ax;Ay;Az\n")
    # Datos de prueba con punto decimal y separador de campos ';'
    f.write("1;0.01;0.05;9.81\n")
    f.write("2;0.03;0.02;9.79\n")
    f.write("3;-0.01;0.04;9.82\n")

print("COMPRUEBO SI FICHERO OK")