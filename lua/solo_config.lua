--Configuration parameters for obtaining the data
local config={
    device1="84:68:3E:00:34:83", --bluetoth name
    device2="84:68:3E:00:34:83",--
    characteristics={--handles of characteristics
	input="0x000b",
	output="0x0011",
    },
    sizeof=1,
}
return config
