--Configuration parameters for obtaining the data
local config={
    device1="84:68:3E:00:34:83", --bluetoth name
    device2="84:68:3E:00:34:5F",--
    characteristics={--handles of characteristics
	input="0x000d",
	output="0x000b",
    },
    sizeof=1,
    hittime=3,
}
return config
