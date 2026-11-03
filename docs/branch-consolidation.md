# Consolidación del desarrollo

Fecha: 2026-11-02 (America/Chihuahua).

## Organización

- `main`: línea estable; no recibe automáticamente el port.
- `develop`: integración Switch/Linux/Android y punto de partida para otra PC,
  móvil y Codex Cloud.
- Nuevos trabajos: ramas temporales feature/* o fix/* desde develop; integrar
  mediante PR hacia develop, validar y retirar la rama temporal.

## Fuentes incorporadas con historial

| Rama de origen | SHA antes de consolidar |
| --- | --- |
| port/linux-native-foundation | `91397c8d415673814eba4e42294e230e7bdbfde4` |
| port/ps-touchscreen-linux-android | `020c77829c4550dad9e49a2094f10ee187dcec66` |
| fix/stability-crash-diagnostics | `73ab6641167a57581f4876878739d724ba5d1b2d` |
| main | `3b6c322f43cfe5c1c8b581838f6be032552951e0` |

La rama más reciente del port ya incluye toda linux-native-foundation.
La integración conserva los commits de ambas líneas y main como ancestros.
Se incorpora la corrección CShader de c9be7d4: liberar shaders antes de sus
CMemPool al cerrar el renderizador. También se conservan DROP_DIAG=1 y
LATENCY_DIAG=1 de 73ab664 en las compilaciones Switch; pueden añadir coste de
diagnóstico. El PR #2 de la base anterior queda sustituido por develop tras
comprobar la consolidación; esto no significa que el port esté fusionado a main.

## Compilaciones y retirada de ramas

Estado de consolidación: completada; CI Linux, Android y Switch exitosa y ramas anteriores retiradas.

Los workflows Linux, Android y Switch aceptan develop. Antes de retirar ramas,
el workflow de consolidación comprueba que los tres resultados corresponden al
mismo SHA de integración y son exitosos. Comprueba también los SHA de origen y
su inclusión como ancestros; si alguien añadió commits a una rama, se detiene
sin retirarla. Las ramas se retiran solo después de todas las comprobaciones.
Main y develop no están en la lista de retirada.

El informe resultante se publica en `docs/consolidation-validation.md` de
develop y main, con enlaces CI y ramas retiradas. Esta retirada borra referencias,
no los commits: se conservan en el historial de develop y en esta tabla.

## Retomar trabajo

Leer [continuidad](CONTINUAR_DESARROLLO.md), [registro](development-log.md) y
AGENTS.md. En una nueva PC clonar con `--branch develop`; en Codex Cloud
seleccionar el mismo repositorio y rama. El entorno Cloud requiere configuración
y publicación desde web/escritorio, y no se ha creado en esta tarea.

La prueba real de VA-API, MediaCodec, táctil, audio, suspensión y fluidez sigue
pendiente en los dispositivos. Las compilaciones CI no la sustituyen.
