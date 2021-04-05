param($xemmai, $xm)
$out = Join-Path (Get-Item .) "$($xm)o"
cd (Split-Path $MyInvocation.MyCommand.Path -Parent)
cat "$($xm)i" | & $xemmai --verbose --debug=$out $xm
if (Select-String -InputObject (cat $out) -Pattern "(?s)$(cat "$($xm)e")") { exit 0 } else { exit 1 }
