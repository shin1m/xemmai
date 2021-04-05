param($xemmai, $xm)
$out = Join-Path (Get-Item .) "$($xm)o"
cd (Split-Path $MyInvocation.MyCommand.Path -Parent)
cmd /c "$xemmai --verbose --debug=$out $xm <$($xm)i"
if (Select-String -InputObject (cat $out) -Pattern "(?s)$(cat "$($xm)e")") { exit 0 } else { exit 1 }
