## Instrucciones

1. Hacer clic en "fork" de este repositorio (botón superior derecho)
2. Abre tu propio fork en Codespaces (botón "Code" > "Codespaces")
3. Ejecuta en la terminal del entorno Codespace:
   ```bash
   # Versión C
   mpicc matmul.c -o matmul_c && mpirun -np 4 ./matmul_c

   # Versión C++
   mpic++ matmul.cpp -o matmul_cpp && mpirun -np 4 ./matmul_cpp

-------------------------------------------------------------------------

## Guardar cambios permanentemente
1. Tras modificar un archivo u otro cambio que quieran hacer, haz **Ctrl+S** en VS Code de Codespace
2. Ejecuta en la terminal:
   ```bash
   git add .
   git commit -m "Mis cambios"
   git push
