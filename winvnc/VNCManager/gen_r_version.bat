@echo off
for /F %%i in ('git rev-parse --short HEAD') do ( set git_id=%%i)
set build_time=%date:~0,4%%date:~5,2%%date:~8,2%.%time:~0,2%%time:~3,2%

if not exist "building.h" goto gen_building

set gen_got=
for /F %%i in ('findstr "%git_id%" building.h') do ( set gen_got=%%i)
if [%gen_got%]==[] goto gen_building

set gen_got=
for /F %%i in ('findstr "%build_time%" building.h') do ( set gen_got=%%i)
if [%gen_got%]==[] goto gen_building

goto over

:gen_building
echo ============ Gen building.h
powershell -Command "(gc building_fmt.h) -replace '/vcs_id/', '%git_id%' | Out-File -encoding ASCII building.h"
powershell -Command "(gc building.h) -replace '/build_time/', '%build_time%' | Out-File -encoding ASCII building.h"

:over

@echo on
