echo off

set SOLUTIONNAME=TvRemote
set PROJECTNAME=TvRemote

set PROJECTSPATH=C:\ZSL\electro\projects
set AVRDUDE="C:\ZSL\electro\bin\avrdude\avrdude.exe"
set HEX=%PROJECTSPATH%\%SOLUTIONNAME%\%PROJECTNAME%\Debug\%PROJECTNAME%.hex"

rem 
%AVRDUDE% -p t13 -c 2ftbb -B 2400 -P ft0 -U flash:w:%HEX%:a

rem �������� ����� � ����������
rem %AVRDUDE% -p t13 -c 2ftbb -B 2400 -P ft0 -U hfuse:r:high.txt:s -U lfuse:r:low.txt:s

rem �������� ��������� ����� �� ����������
rem %AVRDUDE% -p t13 -c 2ftbb -B 2400 -P ft0 -U lfuse:w:0x6a:m -U hfuse:w:0xff:m 

rem �������� ����� �� ����������
rem 0x7A - ��� ATtiny13a (��������� �������� �� 8)
rem 
%AVRDUDE% -p t13 -c 2ftbb -B 2400 -P ft0 -U lfuse:w:0x7A:m

pause