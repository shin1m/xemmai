param($xemmai, $xm)
cmd /c "$xemmai --verbose --debug=NUL $xm <NUL"
exit $LastExitCode
