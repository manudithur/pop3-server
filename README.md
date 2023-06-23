# pop3-server


Integrantes:

-Martin Hecht

-Manuel Dithurbide

-Tomas Gay Bare

-Tobias Perry

## Documentacion

La documentacion se encuentra dentro de la carpeta "docs" en la raiz. Contamos con un diagrama de flujo y el informe del proyecto.

## Compilacion

### Requisitos
- Make
- GCC

### Compilacion

```make all```


Se generaran los binarios `run` y `admin_monitor` dentro del directorio raíz. El primero permite la ejecucion del servidor pop3 mientras que el segundo es la aplicacion cliente del administrador.

## Ejecucion

### Servidor POP3

Se debe correr el comando:

```./run [ARGS]```

Los argumentos deben seguir el siguiente formato:

```./run <ip> <port> -u <username>:<password>```

NOTE: Se pueden agregar multiples usuarios luego del -u separados por un espacio.

### Cliente de monitoreo


```./admin_monitor <version> <username> <password> <command>```

NOTE: por default el username y password del admin son "admin".

Para ver que comandos se pueden correr, se puede correr el ejecutable con el parametro -HELP.

ej: ```./admin_monitor 1.0 admin admin -HELP```
