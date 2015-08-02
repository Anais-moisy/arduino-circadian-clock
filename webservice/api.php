<?php
/**
 * Proxy Webservice that gathers info from sunrise-sunset.org API 
 * and processes the data for the Arduino program
 *
 */
// DateTime config
date_default_timezone_set('Europe/London');
$ISO = 'Y-m-d\TH:i:s.uO';

/**
 * Get the location from the get string or default to Edinburgh
 */
$lat = isset($_GET['lat']) ? $_GET['lat'] : '55.9531';
$lon = isset($_GET['lon']) ? $_GET['lon'] : '-3.1889';
$steps = isset($_GET['steps']) ? $_GET['steps'] : 206;

// ================================================================================= //

/**
 * Get today's sunrise and sunset to compute the duration of the day and the number of steps
 */
$json = file_get_contents("http://api.sunrise-sunset.org/json?lat=$lat&lng=$lon&formatted=0");
$obj = json_decode($json);
$sunrise = DateTime::createFromFormat(DateTime::ISO8601, $obj->results->sunrise);
// echo $sunrise->format('U = Y-m-d H:i:s') . "\n";
$sunset = DateTime::createFromFormat(DateTime::ISO8601, $obj->results->sunset);
// echo $sunset->format('U = Y-m-d H:i:s') . "\n";
$dayLength = diffDates($sunrise, $sunset) * -1000;
$response['dayLength'] = round($dayLength);
$response['daySteps'] = round($dayLength / $steps);


/**
 * Get tomorrow's sunrise to compute the duration of the night and the number of steps
 */
$json = file_get_contents("http://api.sunrise-sunset.org/json?lat=$lat&lng=$lon&formatted=0&date=tomorrow");
$obj = json_decode($json);
$tomorrow = DateTime::createFromFormat(DateTime::ISO8601, $obj->results->sunrise);
// echo $sunrise->format('U = Y-m-d H:i:s') . "\n";
$nightLength = diffDates($tomorrow, $sunset)  * 1000;
$response['nightLength'] = round($nightLength);
$response['nightSteps'] = round($nightLength / $steps);


/**
 * Get the number of steps to initialise the clock to the current time of the day.
 * If it's daytime, compare with sunrise, otherwise sunset.
 */
$now = new DateTime('NOW');
$stepsToInit;
if ($now > $sunset) 
{
	$interval = diffDates($now, $sunset);
	$stepsToInit = $interval * 1000 * $steps / $nightLength;
	// echo "since sunset : ". $stepsToInit;
}
else 
{
	$interval = diffDates($now, $sunrise);
	$stepsToInit =  $interval * 1000 * $steps / $dayLength;
	// echo "since surise : ". $stepsToInit;
}
$response['stepsToInit'] = round($stepsToInit);

// Return as JSON
header('Content-type: application/json');
echo json_encode($response);

// ================================================================================= //

/**
 * @param DateTime $sunrise
 * @param DateTime $sunset
 * @return int seconds
 */
function diffDates($sunrise, $sunset) {
	$diff = $sunrise->getTimestamp() - $sunset->getTimestamp();
	return $diff;
}


?>