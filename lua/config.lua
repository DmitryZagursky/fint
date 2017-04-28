--Configuration parameters for obtaining the data
local config={
    device="84:68:3E:00:34:83", --bluetoth name
    characteristics={--handles of characteristics
	input="0x000b",
	index="0x000d",
	output="0x00f",
    },
}
return config
