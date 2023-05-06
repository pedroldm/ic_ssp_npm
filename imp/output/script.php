<?php
$objectives = ['TS', 'FMAX', 'TFT'];
$regex = ['#TOTAL\s*TOOL\s*SWITCHES\s*\:\s*(\d+)#is', '#HIGHEST\s*MAKESPAN\s*:\s*(\d+)#is', '#TOTAL\s*FLOWTIME\s*\:\s*(\d+)#is'];
$instance_sets = ['m2j10t10', 'm2j10t15', 'm2j15t10', 'm2j15t15', 'm3j15t15', 'm3j15t20', 'm3j20t15', 'm3j20t20', 'm4j40t120', 'm4j40t60', 'm4j60t120', 'm4j60t60', 'm6j120t120', 'm6j80t120'];

foreach($instance_sets as $i) {
    foreach($objectives as $k => $obj) {
        $files = glob("$i/$obj/o*.txt");
        printResultsSummary($files, $i, $k, $obj);
    }
}


function printResultsSummary($files, $ins_set, $k, $obj) {
    if(!count($files))
        return;

    global $regex;
    $results = [];
    
    foreach($files as $f) {
        $f = file_get_contents($f);
        preg_match_all($regex[$k], $f, $r);
        array_push($results, $r[1]);
    }
    
    $sum = 0;
    $med = 0;
    for($i = 0 ; $i < count($results[0]) ; $i++) {
        $min = 999999;
        for($j = 0 ; $j < count($results) ; $j++) {
            $med += $results[$j][$i];
            if($results[$j][$i] < $min)
                $min = $results[$j][$i];
        }
        $sum += $min;
    }
    $media_melhores = round($sum / count($results[0]));
    $media_geral = round($med / (count($results[0]) * count($results)));

    print_r($ins_set . "\t" . $obj . "\t" . $media_melhores . "\t" . $media_geral . "\n");
}

function standard_deviation($arr) {
    $variance = 0.0;
    $average = array_sum($arr)/count($arr);
        
    foreach($arr as $i)
        $variance += pow(($i - $average), 2);
        
    return (float)sqrt($variance/count($arr));
}