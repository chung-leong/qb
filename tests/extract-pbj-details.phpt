--TEST--
PBJ info extraction test
--FILE--
<?php

$path = "pbj/test.pbj";
$info = qb_extract($path, QB_PBJ_DETAILS);
print_r($info);

?>
--EXPECT--
Array
(
    [vendor] => Test Vendor
    [name] => test
    [displayName] => Display name
    [description] => This is a description
    [version] => 4
    [parameters] => Array
        (
            [0] => Array
                (
                    [direction] => in
                    [name] => value1
                    [displayName] => Value #1
                    [type] => int
                    [parameterType] => percentage
                    [description] => Description #1
                    [minValue] => 1
                    [maxValue] => 100
                    [defaultValue] => 50
                )

            [1] => Array
                (
                    [direction] => in
                    [name] => value2
                    [displayName] => Value #2
                    [type] => float2
                    [parameterType] => position
                    [description] => Description #2
                    [minValue] => Array
                        (
                            [0] => -300
                            [1] => -300
                        )

                    [maxValue] => Array
                        (
                            [0] => 900
                            [1] => 900
                        )

                    [defaultValue] => Array
                        (
                            [0] => 50
                            [1] => 200
                        )

                )

            [2] => Array
                (
                    [direction] => in
                    [name] => value3
                    [displayName] => Value #3
                    [type] => float3x3
                    [parameterType] => 
                    [description] => Description #3
                    [minValue] => Array
                        (
                            [0] => 1
                            [1] => 2
                            [2] => 3
                            [3] => 4
                            [4] => 5
                            [5] => 6
                            [6] => 7
                            [7] => 8
                            [8] => 9
                        )

                    [maxValue] => Array
                        (
                            [0] => 100
                            [1] => 200
                            [2] => 300
                            [3] => 400
                            [4] => 500
                            [5] => 600
                            [6] => 700
                            [7] => 800
                            [8] => 900
                        )

                    [defaultValue] => Array
                        (
                            [0] => 10
                            [1] => 20
                            [2] => 30
                            [3] => 40
                            [4] => 50
                            [5] => 60
                            [6] => 70
                            [7] => 80
                            [8] => 90
                        )

                )

            [3] => Array
                (
                    [direction] => in
                    [name] => value4
                    [displayName] => Value #4
                    [type] => float4
                    [parameterType] => colorRGBA
                    [description] => Description #4
                    [minValue] => Array
                        (
                            [0] => 0
                            [1] => 0
                            [2] => 0
                            [3] => 0
                        )

                    [maxValue] => Array
                        (
                            [0] => 1
                            [1] => 1
                            [2] => 1
                            [3] => 1
                        )

                    [defaultValue] => Array
                        (
                            [0] => 0.5
                            [1] => 0.5
                            [2] => 0.5
                            [3] => 0.5
                        )

                )

            [4] => Array
                (
                    [direction] => out
                    [name] => dst
                    [displayName] => 
                    [type] => float4
                    [parameterType] => 
                    [description] => 
                )

            [5] => Array
                (
                    [direction] => in
                    [name] => src
                    [displayName] => 
                    [type] => image4
                    [parameterType] => 
                    [description] => 
                )

        )

)