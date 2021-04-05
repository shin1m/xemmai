param($xemmai, $xm)
$out = cmd /c "$xemmai $xm 2>&1"
if (Select-String -InputObject $out -Pattern "(?s)$(cat "$($xm)e")") { exit 0 } else { exit 1 }
