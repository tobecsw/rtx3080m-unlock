@echo off
setlocal
cd /d "%~dp0"
where bun >nul 2>nul
if errorlevel 1 (
  echo Bun is required. Install it from https://bun.sh and reopen this script.
  pause
  exit /b 1
)
call bun install --frozen-lockfile
if errorlevel 1 goto failed
call bun tools/firmware/build_assets.ts RTX-UNLOCK/ASSETS
if errorlevel 1 goto failed
echo Firmware assets generated locally. Keep the NVIDIA notice with the assets.
pause
exit /b 0
:failed
echo Firmware preparation failed. Read the error above; do not boot an incomplete package.
pause
exit /b 1
