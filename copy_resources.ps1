param (
	[Parameter(Mandatory=$true)][string]$in,
	[Parameter(Mandatory=$true)][string]$out
)

$fullname_in = gi $in | select -exp fullname
$fullname_out = gi $out | select -exp fullname

if (!(test-path $fullname_in)) {
	write "Input directory not found"
	return
}

# -file added to powershell in 3.0, as far as I can tell
$version = (get-host).version | select -exp major
if ($version -ge 3) {
	$results = gci -recurse -file $in
} else {
	$results = gci -recurse $in | where {$_ -is [system.io.fileInfo]}
}


$results | % {
	$fullname_file = $_.fullname
	if (!$fullname_file.startsWith($fullname_in)) {
		write "[-] Error for file $fullname_file."
		return
	}
	$relative_path = $fullname_file.substring($fullname_in.length)
	$out_path = $fullname_out + $relative_path

	if (test-path $out_path) {
		$in_time = $_.lastWriteTime
		$out_time = gi $out_path | select -exp lastWriteTime
		if ($in_time -gt $out_time) {
			copy $fullname_file -destination $out_path
			write "[+] Updated $fullname_file."
		}
	} else {
		$name_file = $_.name
		$relative_directory = $relative_path.substring(0, $relative_path.length - $name_file.length)
		$out_directory = $fullname_out + $relative_directory
		if (!(test-path $out_directory)) {
			mkdir $out_directory | out-null
		}
		copy $fullname_file -destination $out_path
		write "[+] Copied $fullname_file."
	}
}
