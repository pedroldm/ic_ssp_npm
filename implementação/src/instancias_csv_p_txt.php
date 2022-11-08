<?php

$files = glob('*.csv', 0);

foreach($files as $f) {
    $csv = file_get_contents($f);
    $csv = preg_replace('#\;NA#is', '', $csv);
    $csv = preg_replace('#\;#is', ' ', $csv);
    file_put_contents($f, $csv);
}