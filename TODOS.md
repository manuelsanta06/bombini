# TODOs de Bombini

Documento de trabajo basado en la revisión del proyecto del 25/26 de agosto de 2026.

## Estado general

La arquitectura actual es una buena base: el proyecto está dividido entre CLI, indexador, búsqueda fuzzy y daemon; mantiene un caché en memoria; soporta fallback sin daemon; y ya incluye Makefile, servicio systemd, README y licencia MIT.

## Prioridad 1: robustez y seguridad

### Servicio systemd

- [ ] Hacer coincidir `ExecStart` con el destino real de instalación. El Makefile instala por defecto en `/usr/local/bin`, mientras que `bombini.service` ejecuta `/usr/bin/bombini`.
- [ ] Usar una plantilla de servicio o una variable de instalación para generar el path correcto.
- [ ] Revisar si `After=graphical-session.target` es realmente necesario para un servicio de usuario.
- [ ] Documentar claramente si `make install` requiere privilegios y cómo se recarga el servicio después de actualizarlo.

### Socket Unix

- [ ] No ejecutar `unlink()` sobre el socket antes de comprobar si ya existe un daemon. Puede desconectar un daemon activo y permitir dos instancias.
- [ ] Mover el socket desde `/tmp/bombini.sock` a `$XDG_RUNTIME_DIR/bombini.sock`.
- [ ] Configurar permisos del socket para que solo el usuario pueda conectarse.
- [ ] Inicializar completamente `sockaddr_un`, validar el largo de la ruta y usar un largo de `bind()` portable.
- [ ] Cerrar y eliminar el socket al recibir `SIGTERM` o `SIGINT`.
- [ ] Usar `SOCK_CLOEXEC`/`FD_CLOEXEC` para no filtrar descriptores a procesos hijos.

### Protocolo cliente-daemon

- [ ] Definir un protocolo explícito con límite de tamaño y delimitación de mensajes.
- [ ] Implementar lectura y escritura completas: una llamada a `read()` o `write()` no garantiza transferir todo el mensaje.
- [ ] Agregar timeouts para que un cliente conectado pero inactivo no bloquee el daemon.
- [ ] Evitar truncar consultas largas silenciosamente.
- [ ] Comprobar el resultado de `socket()`, `listen()`, `read()`, `write()` y `snprintf()`.
- [ ] Evitar que un `SIGPIPE` termine el daemon si el cliente se desconecta durante una respuesta.
- [ ] Devolver respuestas de error estructuradas y códigos de estado claros.

## Prioridad 2: datos correctos

### Parser de archivos `.desktop`

- [ ] Parsear pares clave/valor en vez de buscar texto con `strncmp()` y `strstr()`.
- [ ] Interpretar correctamente `Type=Application`, `Terminal=true/false` y `NoDisplay=true`.
- [ ] Soportar `Hidden`, `OnlyShowIn`, `NotShowIn` y `TryExec`.
- [ ] Soportar nombres localizados como `Name[es]=...` respetando el locale.
- [ ] Usar `getline()` o una estrategia dinámica para no limitar cada línea a 511 caracteres.
- [ ] Mejorar el parseo del campo `Exec`, incluyendo comillas y códigos `%f`, `%F`, `%u`, `%U`, etc.
- [ ] Definir correctamente la precedencia cuando el mismo desktop ID aparece en varios directorios.
- [ ] Detectar y reportar paths truncados o archivos inválidos.

### JSON y formato plain

- [ ] Escapar caracteres de control JSON (`\\n`, `\\r`, `\\t`, `\\b`, `\\f` y bytes menores a `0x20`).
- [ ] Eliminar buffers fijos de 1024 bytes o detectar el truncamiento antes de devolver resultados.
- [ ] Escapar tabs y saltos de línea en el formato plain, o documentar que no admite esos valores.
- [ ] Añadir orden estable para resultados con el mismo score.

## Prioridad 3: calidad del código

- [ ] Cambiar `int` por `size_t` en cantidades y capacidades.
- [ ] No perder el puntero original si falla `realloc()`.
- [ ] Evitar `abort()` para errores recuperables; devolver errores al nivel que pueda mostrarlos.
- [ ] Corregir el comparador de resultados para no restar enteros y provocar overflow.
- [ ] Revisar la complejidad del fuzzy matcher recursivo para consultas largas o repetitivas.
- [ ] Usar `const` donde corresponda y hacer `static` las funciones internas.
- [ ] Unificar estilo: nombres, espacios, llaves, idioma de comentarios y mensajes.
- [ ] Revisar el orden de las opciones CLI: actualmente `--standalone --set-path ...` puede comportarse distinto de `--set-path ... --standalone`.
- [ ] Usar nombres convencionales como `--standalone`, `--set-path` y `--add-path`, manteniendo aliases si hace falta.

## Prioridad 4: build, tests y mantenimiento

- [ ] Agregar dependencias automáticas de headers (`-MMD -MP`).
- [ ] Declarar objetivos `.PHONY` completos, incluyendo `test` cuando exista.
- [ ] Añadir objetivos `debug`, `asan`, `ubsan`, `format` y `check`.
- [ ] Crear tests unitarios para fuzzy matching, parser, JSON y protocolo.
- [ ] Crear tests de integración que levanten el daemon, consulten, recarguen y lo detengan.
- [ ] Ejecutar GCC y Clang en CI con sanitizers y análisis estático.
- [ ] Añadir `CONTRIBUTING.md` y documentar el protocolo del socket en el README.
- [ ] Revisar afirmaciones como “zero latency” y “microseconds” usando benchmarks reproducibles.
- [ ] Mantener el README sincronizado con los nombres reales de las opciones y el proceso de instalación.

## Plan sugerido

1. Corregir instalación systemd y ciclo de vida del socket.
2. Definir un protocolo robusto con límites, timeouts y errores.
3. Corregir serialización JSON y truncamientos.
4. Mejorar el parser `.desktop` con fixtures de prueba.
5. Añadir tests, sanitizers, CI y formato automático.
6. Medir y optimizar la búsqueda solo después de tener pruebas de corrección.

