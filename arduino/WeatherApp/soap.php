<?php
// Read the online wsdl and create a client for it. The second (array trace, stream_context
// parameter simply keeps the client from timing out (incorrectly accepting the XML response)
$client = new SoapClient("http://graphical.weather.gov/xml/DWMLgen/wsdl/ndfdXML.wsdl",
                         array('trace'=>1,
                               'stream_context'=>stream_context_create(array('http'=>array('protocol_version'=>1.0)))
                               )
                        );
// Our first query to the database is going to be to get the lat/lon for our zipcode.
// This is the array, containing our zipcode, that we'll send to the datase.
$getZip = array('zipCodeList'=>"44106");
// Our second query to the database will be to grab the actual weather data for our
// area (the lat/lon determined by the zip code).  This long list is the array of
// possible parameters for the online service.
$weatherParams = array('maxt'=>'1',
                       'mint'=>'1',
                       'temp'=>'1',
                       'dew'=>'0',
                       'appt'=>'1',
                       'pop12'=>'1',
                       'qpf'=>'1',
                       'snow'=>'0',
                       'sky'=>'0',
                       'rh'=>'1',
                       'wspd'=>'0',
                       'wdir'=>'0',
                       'wx'=>'1',
                       'icons'=>'1',
                       'waveh'=>'0',
                       'incw34'=>'0',
                       'incw50'=>'0',
                       'incw64'=>'0',
                       'cumw34'=>'0',
                       'cumw50'=>'0',
                       'cumw64'=>'0',
                       'wgust'=>'0',
                       'critfireo'=>'0',
                       'dryfireo'=>'0',
                       'conhazo'=>'0',
                       'ptornado'=>'0',
                       'phail'=>'0',
                       'ptstmwinds'=>'0',
                       'pxtornado'=>'0',
                       'pxhail'=>'0',
                       'pxtstmwinds'=>'0',
                       'ptotsvrtstm'=>'0',
                       'pxtotsvrtstm'=>'0',
                       'tmpabv14d'=>'0',
                       'tmpblw14d'=>'0',
                       'tmpabv30d'=>'0',
                       'tmpblw30d'=>'0',
                       'tmpabv90d'=>'0',
                       'tmpblw90d'=>'0',
                       'prcpabv14d'=>'0',
                       'prcpblw14d'=>'0',
                       'prcpabv30d'=>'0',
                       'prcpblw30d'=>'0',
                       'prcpabv90d'=>'0',
                       'prcpblw90d'=>'0',
                       'precipa_r'=>'0',
                       'sky_r'=>'0',
                       'td_r'=>'0',
                       'temp_r'=>'0',
                       'wdir_r'=>'0',
                       'wspd_r'=>'0',
                       'wwa'=>'1',
                       'iceaccum'=>'0',
                       'maxrh'=>'0',
                       'minrh'=>'0');

// This is the first call, which returns our lat/lon
$latLonList = $client->__soapCall("LatLonListZipCode", $getZip);

if (preg_match('/<latLonList>(.*)<\/latLonList>/', $latLonList, $matches)){
  // print("regex works: " . $matches[1] . "\n");
  $strArr = explode(',',$matches[1]);
  $lat = $strArr[0];
  $long = $strArr[1];
}

// This is the actual array of parameters that will be passed to the database
// in the second request.  It contains the above array, along with the needed
// contextual parameters.
$weatherParams2 = array('latitude'=>$lat,
                        'longitude'=>$long,
                        'product'=>'time-series',
                        'startTime'=>'',
                        'endTime'=>'',
                        'Unit'=>'e',
                        'weatherParameters'=>$weatherParams);

//This is the second call, which returns current weather information
$response = $client->__soapCall("NDFDgen",$weatherParams2);

//Output response to file
$file = fopen("ndfd_info.xml", "w");
fwrite($file, $response);
fclose($file);

// Debugging below:
// Print the first response
var_dump($latLonList);
print ("\n");
// Print the second response
var_dump($response);
?>