# Continuar DualStream / LunarNX desde otra sesión

Última actualización: 2026-11-03 (America/Chihuahua).

Novedades (rama `manuelgzz-ps-auto-search-lan`, pendiente de CI y merge a
develop): al abrir PlayStation se ejecuta automáticamente "Search LAN" en
segundo plano en las tres compilaciones. Pruebas en Android: Xbox más estable
por software, PlayStation mejor con MediaCodec, gamepads táctiles correctos.
Abierto: en Linux con PlayStation el indicador siempre muestra SW.
Repositorio de trabajo: https://github.com/manuelgzz/LunarNX

## Leer primero

El desarrollo integrado Switch/Linux/Android continúa en **develop**.
`main` conserva la línea estable. Las ramas del port y diagnóstico se han
incorporado con su historial en develop; su retirada se condiciona a las tres
compilaciones exitosas. Consultar [consolidación](branch-consolidation.md).
La copia de esta guía en main sirve de entrada; el estado vigente está en develop.

Leer en este orden: `AGENTS.md`, este documento,
[registro de cambios](development-log.md), [empaquetado](https://github.com/manuelgzz/LunarNX/blob/develop/docs/packaging.md),
[Linux](https://github.com/manuelgzz/LunarNX/blob/develop/docs/linux-development.md) y [dirección del port](https://github.com/manuelgzz/LunarNX/blob/develop/docs/linux-android-port.md).
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
git clone --branch develop https://github.com/manuelgzz/LunarNX.git
cd LunarNX
git switch develop
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
2. Conectar GitHub, seleccionar manuelgzz/LunarNX y usar la rama develop al
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
