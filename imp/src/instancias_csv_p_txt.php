<?php

$files = glob('*.csv', 0);

foreach($files as $f) {
    $csv = file_get_contents($f);
    $csv = preg_replace('#\;NA#is', '', $csv);
    $csv = preg_replace('#\;#is', ' ', $csv);
    $txt = substr($f, 0, -3) . 'txt';
    shell_exec("rm $f");
    file_put_contents($txt, $csv);
}