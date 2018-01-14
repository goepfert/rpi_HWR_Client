#include <map>
#include <string>

std::map<std::string, std::string> devicemap;

void create_map();

void create_map() {
	devicemap["28-00000735ad3b"] = "temp1";
	devicemap["28-00000735d381"] = "temp2";
	devicemap["28-00000735a638"] = "temp3";
	devicemap["28-00000736500d"] = "temp4";
	devicemap["28-00000735c405"] = "temp5";
	devicemap["28-0000073642ef"] = "temp6";
	devicemap["28-00000743bb2e"] = "temp7";
	devicemap["28-000007365685"] = "temp8";
	devicemap["28-00000736299a"] = "temp9";
	devicemap["28-000000000000"] = "gas_counter";
	devicemap["28-000000000001"] = "gas_rate";
	devicemap["28-000000000002"] = "elt_counter";
	devicemap["28-000000000003"] = "elt_rate";
}
