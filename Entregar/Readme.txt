Este documento de texto pretende explicar como ejecutar los códigos y el contendio de los diferentes directorios.

En el directorio Documentos encontramos los resultados de la ejecución Resutlado1.txt y Resutlado2.txt para ambas
versiones del porblema. También encontramos el documento explicativo de la práctica. Adicionalmente se ha añadido
un Directorio Output donde en su interior podemos encontrar los resultados de tiempos de ejecución y el resultado
especifico de cada problema estructurado en directorios  y para las 2 alternativas de resolucion planteadas.
Hay un directorio MaximumBipartiteMatcing y otro CirculationWithDemands. Dentro de cada direcotrio encontramos las
versiones y los algoritmos.

En el directorio Codigo hay un makefile que permite ejecutar ambas implementaciones, el makefile tiene 4 comandos:
-build1: Compila la version MaximumBipartiteMatching
-build2: Compila la version CirculationWithDemands
-run1: Ejecuta la version MaximumBipartiteMatching requiere 3 argumentos x=? y=? z=?
    ejemplo: make run1 1 2 nombre_fichero(sin el .air)
-run2: Ejecuta la version CirculationWithDemands requiere 3 argumentos x=? y=?
-clean: limpia el ejecutable

Alternativamente al run podemos haver ./programa arg1 arg2 ...
