# Embedded Directory

## Testing And Coverage

The test setup uses a dedicated host environment (`env:native`) in
`platformio.ini`. Tests are compiled and executed on the CI runner and are
never included in the firmware image built for `wemos_d1_mini32`.

Run tests locally:

```bash
pio test -e native
```

Generate function coverage for project sources:

```bash
gcovr --root . --object-directory .pio/build/native --filter src --filter lib --txt --txt-metric function
```