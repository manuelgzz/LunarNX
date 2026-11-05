# Continuar DualStream / LunarNX desde otra sesión

Última actualización: 2026-11-03 (America/Chihuahua).

Novedades: todo el trabajo (port Switch/Linux/Android, gamepads táctiles, selector de
decoder, Search LAN automático en PlayStation, firma estable de Android) está
consolidado en **main** de manuelgzz/LunarNX (sin PR al repositorio original).
Pruebas en Android: Xbox más estable por software, PlayStation mejor con
MediaCodec. Abierto: en Linux con PlayStation el indicador siempre muestra SW
(pendiente del log [video-decode]). Generación de frames (LSFG) en Android: probada y cancelada por decisión del usuario; la rama feature/android-frame-generation se eliminó (sin fusionar). Siguiente: validar en hardware y el indicador SW en Linux PS.
Releases: release-please (PR automático, fusionarlo publica) adjunta NRO, APK y AppImage con SHA256SUMS.
Repositorio de trabajo: https://github.com/manuelgzz/LunarNX

## Leer primero

El desarrollo integrado Switch/Linux/Android está ahora en **main**; developse conserva como referencia histórica. Nuevo desarrollo: ramas cortas desde main.
Consultar [consolidación](branch-consolidation.md).

Leer en este orden: `AGENTS.md`, este documento,
[registro de cambios](development-log.md), [empaquetado](https://github.com/manuelgzz/LunarNX/blob/main/docs/packaging.md),
[Linux](https://github.com/manuelgzz/LunarNX/blob/main/docs/linux-development.md) y [dirección del port](https://github.com/manuelgzz/LunarNX/blob/main/docs/linux-android-port.md).
Los tres últimos documentos están en la rama del port.

DualStream es el nombre usado en las conversaciones para el objetivo de unificar
Xbox y PlayStation. El repositorio, ejecutables y paquetes todavía usan LunarNX.
El lanzador Python Dualstream es una implementación separada: no confundirlo con
este port nativo ni migrarlo sin una decisión explícita.

## Estado verificado en GitHub

Referencia de código revisada: `59a3efec43854cf5ad3766af0fbb77f673a1cac2`.
Esta es la base histórica validada antes de la consolidación. Develop añade la
corrección de cierre del renderizador Switch (c9be7d4), diagnósticos (73ab664),
las reglas CI y esta documentación. Las ejecuciones siguientes pertenecen a la
base histórica; consultar el registro de consolidación para las nuevas.

- Interfaz Borealis propia con motores Xbox y PlayStation integrados; no abre
  Chiaki-ng ni XStreamingDesktop como aplicaciones externas.
- Paquetes de desarrollo: AppImage Linux x86_64 y APK Android ARM64.
- Linux intenta VA-API y Android MediaCodec para H.264/HEVC, con recuperación
  por CPU. Hay un intento de aceleración por sesión.
- Presentación mediante RGBA y subida a la interfaz; no es zero-copy.
- El indicador VA-API / MediaCodec / SW se actualiza al producir una imagen.
  MediaCodec no demuestra por sí solo que el dispositivo haya elegido un codec
  físico del fabricante.
- Los últimos cambios corrigen contactos, clics y reemplazo de contactos del
  touchpad PlayStation en pantallas táctiles Linux/Android.
- Se conservan los límites de memoria y las restricciones Switch de AGENTS.md.

Resultados consultados en esta actualización, ambos exitosos para esa referencia:

| Plataforma | Ejecución | Artefacto |
| --- | --- | --- |
| Linux | [41537165614](https://github.com/manuelgzz/LunarNX/actions/runs/41537165614) | LunarNX-Linux-AppImage-x86_64 |
| Android | [41537165550](https://github.com/manuelgzz/LunarNX/actions/runs/41537165550) | LunarNX-Android-ARM64-APK |

Los artefactos tienen caducidad; no son releases permanentes. Incluyen fuentes
correspondientes. Si ya no están disponibles, reconstruir desde una referencia
identificada usando los workflows existentes.

CI Linux valida pruebas portables, decodificación y recuperación por CPU,
mock Xbox, reproducción PlayStation H.264/HEVC, interfaz y AppImage.
CI Android valida compilación, enlace, firma y contrato del APK. Consultar los
pasos de cada ejecución antes de atribuirle una prueba concreta. Ninguno
demuestra fluidez o aceleración física en la tablet o ROG Ally.

## Validación de la consolidación

Estado de consolidación: completada; CI Linux, Android y Switch exitosa y ramas anteriores retiradas.

Las tres compilaciones se ejecutan en develop: Linux, Android y Switch con
pruebas host y comprobación BSS. El workflow de consolidación en main retirará
solo las ramas registradas cuyo SHA no haya cambiado y cuyo historial esté
contenido en el commit validado. El informe final queda en
`docs/consolidation-validation.md` cuando finaliza. Si algo falla, conservar
las ramas y consultar Actions antes de continuar.

## Control de decodificación nativa (Linux/Android)

En la rama `feature/native-decoder-selection`, los ajustes de Xbox y PlayStation
ofrecen selecciones independientes: Automático (intenta VA-API en Linux o
MediaCodec en Android, con recuperación por CPU) y Software (CPU). Se guardan
como `xbox_native_decode_mode` y `ps_native_decode_mode`, y se aplican al iniciar
la siguiente sesión; no cambian el decoder durante una transmisión activa.
Los archivos existentes mantienen Automático sin migración. El override de
pruebas `LUNARNX_VIDEO_DECODE=software` sigue forzando CPU.

Ajustes generales y el menú durante el juego permiten mostrar/ocultar el
indicador del decoder activo, independientemente de las estadísticas. La
preferencia compartida `show_decoder_overlay` queda guardada y empieza oculta.
El indicador muestra `...` antes de producir imagen, después VA-API, MediaCodec
o SW, según el decoder que entrega cuadros. MediaCodec no acredita por sí solo
un codec físico del fabricante. Switch conserva sus modos anteriores.

Validación local: Linux compilado en Docker Ubuntu 24.04, 4 pruebas nativas y
11 portables exitosas, smoke de decodificación H.264/HEVC y audio en cuatro
modos de CPU/recuperación, smoke de ajustes y reproducción PS H.264 a 720p
con indicador habilitado. Detalles en development-log.md. Android APK y
Switch NRO no compilados en esta sesión; sin validación nueva en hardware.
La rama está publicada con autorización del usuario en el
[PR #3 hacia develop](https://github.com/manuelgzz/LunarNX/pull/3).
Linux, Android y Switch se validan mediante sus workflows del PR; consultar
[ejecuciones de la rama](https://github.com/manuelgzz/LunarNX/actions?query=branch%3Afeature%2Fnative-decoder-selection)
para resultados y artefactos de cada SHA. La compilación local no sustituye
el resultado del APK/NRO ni las pruebas en los dispositivos.

### Pruebas de usuario de la compilación anterior

El usuario informa uso de la última compilación disponible (sin SHA identificado),
a 720p, consolas por cable y tablet Android/ROG Ally Linux por Wi-Fi:

| Plataforma | PlayStation | Xbox |
| --- | --- | --- |
| Android | Muy estable | Mejoría considerable, microcongelamientos muy ocasionales |
| Linux | Estable, microcongelamientos ocasionales | Estable, microcongelamientos frecuentes |

Son observaciones reales del usuario; aún no identifican decoder activo, FPS,
bitrate o causa de las pausas. No son validación en hardware de este nuevo cambio.
Prioridad: comparar Automático y Software en Xbox/Linux, después PS/Linux y
Xbox/Android, con el indicador visible y las mismas condiciones de red/calidad.

## Gamepad táctil Linux/Android

El PR #3 también incorpora un gamepad táctil con perfiles Xbox y PlayStation.
En ajustes de cada plataforma → controles: activar/desactivar y opacidad
10/25/50/75/100%; ambas preferencias son independientes entre Xbox y PS.
Predeterminado oculto, opacidad 50%. En una sesión se accede con `...` arriba a
la derecha → ajustes; guardar y volver aplica visibilidad/opacidad sin reiniciar.

Incluye sticks, cruceta, botones frontales, hombros/gatillos, clics de sticks,
View/Menu/Xbox o Share/Options/PS, y clic de touchpad PS. Mientras está visible,
PS usa únicamente el rectángulo central como touchpad; ocultarlo recupera la
pantalla completa. Los dedos se asignan al control en el que empezaron y no
saltan entre botones/sticks y touchpad al arrastrar. Abrir menús, perder foco y
cambiar de modo suelta los controles y exige levantar los dedos retenidos.
El touchpad del mando físico sigue disponible. Xbox captura y micrófono/mute
DualSense no están soportados por la ruta de entrada/audio actual.

Validación de este incremento: compilación Linux Docker, 4 pruebas nativas y
12 portables exitosas, navegación de ajustes y replay PS H.264 720p con gamepad.
Pendiente nueva CI Android/Switch y pruebas multitáctiles reales en tablet/Ally.
Las nuevas opciones no modifican el lector HID/touchpad de Switch. Ver registro
de desarrollo y guía de empaquetado para comportamiento y límites.

## Próximo trabajo

1. Probar el AppImage en la ROG Ally con Linux y el APK en la tablet Android.
   Registrar versión/commit, sistema, plataforma Xbox/PS, codec, resolución,
   indicador VA-API/MediaCodec/SW, fluidez, audio y controles.
2. Comprobar recuperación por CPU y comparar rendimiento; Linux permite
   `LUNARNX_VIDEO_DECODE=software`. No declarar aceleración física validada
   hasta observarla en el equipo.
3. Probar touchpad PS con uno/dos dedos, clic corto, pulsación sostenida,
   reemplazo de contactos y abrir/cerrar el menú sin dejar contactos atascados.
4. Validar autenticación real, streaming LAN/PSN, reconexión y suspensión.
5. Linux + PlayStation muestra siempre SW: compartir las líneas `[video-decode]`
   de `~/.cache/lunarnx/lunarnx.log` tras iniciar un stream PS (y uno Xbox para
   comparar) con la nueva compilación; decidir la corrección con ese motivo.
6. Verificar en hardware la búsqueda LAN automática al abrir PlayStation.
7. Corregir problemas reproducibles con pruebas enfocadas y actualizar estas
   notas y el registro al terminar cada cambio.

El APK de desarrollo ahora se firma con una clave estable (secretos de GitHub):
tras desinstalar una última vez, las actualizaciones conservan configuración y
cuentas. Ver [empaquetado](packaging.md). No publicar credenciales para
reproducirlo.

## Continuar en otra PC

```sh
git clone --branch main https://github.com/manuelgzz/LunarNX.git
cd LunarNX
git switch main
git status --short --branch
git log -5 --oneline
```

Si ya existe el checkout, revisar primero cambios locales; después hacer fetch
y cambiar a la rama sin sobrescribir trabajo. Preparar herramientas según los
documentos de plataforma y los workflows; clonar no instala dependencias.

Texto para iniciar una sesión nueva:

> Continúa el desarrollo de manuelgzz/LunarNX en la rama
> develop. Lee AGENTS.md,
> docs/CONTINUAR_DESARROLLO.md y docs/development-log.md. Revisa los últimos
> commits y GitHub Actions antes de cambiar código. Retoma los pendientes
> documentados y actualiza el estado y el registro al terminar cada cambio.

El historial local de Codex no se recupera solo con nombrar el repositorio.
GitHub conserva archivos y commits publicados, no cambios locales sin subir,
herramientas instaladas ni credenciales. Estas notas reemplazan la dependencia
de una conversación para retomar el desarrollo.

## Preparar trabajo en la nube y móvil

No se ha creado ni publicado un entorno Codex Cloud como parte de esta
documentación. GitHub Actions ya compila en la nube, pero eso es distinto de
un entorno de desarrollo Codex Cloud.

Desde web/escritorio, si la función está disponible en la cuenta:

1. Crear un entorno en Work in > Cloud > Create environment.
2. Conectar GitHub, seleccionar manuelgzz/LunarNX y usar la rama main al
   iniciar la tarea; comprobar la rama y el commit dentro del entorno.
3. Preparar dependencias siguiendo los workflows Linux/Android y sus versiones
   fijadas. Para Switch se necesita Docker/devkitA64; verificar disponibilidad.
4. Ejecutar pruebas apropiadas, revisar la configuración y publicar el entorno.
5. Abrir Codex desde un dispositivo compatible, elegir el entorno publicado y
   continuar la tarea con el texto anterior.

No asumir que el identificador de esta sesión local importa el chat a Cloud.
El código y estas notas aportan el contexto para una tarea nueva. Las pruebas
de GPU, táctil y streaming en dispositivos reales siguen necesitando hardware.

Guía oficial: https://learn.chatgpt.com/docs/environments/cloud-environments

## Mantener la continuidad

Después de cada cambio terminado, actualizar este estado y
`docs/development-log.md` en el mismo commit cuando sea posible: comportamiento,
archivos relevantes, comandos de validación y resultado, limitaciones, siguiente
paso y enlaces a commits/CI/artefactos cuando existan. Actualizar las guías
afectadas. Distinguir implementado, validado en CI y validado en hardware.
Si un dato no se verificó, decirlo. No inventar resultados ni publicar tokens,
cuentas, IP privadas, logs completos o conversaciones sin depurar.
