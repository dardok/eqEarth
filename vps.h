#include <osg/Vec3d>
#include <osgEarth/Viewpoint>

static osgEarth::Viewpoint VPs[] = {
osgEarth::Viewpoint( "FtDix", osg::Vec3d( -74.615666944444442, 39.997432777777782, 0 ), 0.0, -90, 1e3 ),
osgEarth::Viewpoint( "FtOrdMOUT", osg::Vec3d( -121.8091, 36.5824, 0 ), 0.0, -90, 1e3 ),
osgEarth::Viewpoint( "Pyong Yang, KN", osg::Vec3d( 125.762933582303, 39.0339236242285, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Quebec, CA", osg::Vec3d( -71.2562079838935, 46.8295758596705, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Quito, EC", osg::Vec3d( -78.5032399097136, -0.211533019512086, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Rangoon, BM", osg::Vec3d( 96.1441603850067, 16.8515443420177, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Regina, CA", osg::Vec3d( -104.615790965834, 50.4509160227975, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Reykjavik, IC", osg::Vec3d( -21.8955382060806, 64.109410117365, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Riga, LG", osg::Vec3d( 24.1056277780126, 56.9811326989319, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Rio de Janeiro, BR", osg::Vec3d( -43.3316315979071, -22.8486738102976, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Riyadh, SA", osg::Vec3d( 46.7229439540625, 24.6763296205535, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Rome, IT", osg::Vec3d( 12.507375815398, 41.8944654243864, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Salt Lake City, US", osg::Vec3d( -111.948832972351, 40.7113786088403, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Sana, YM", osg::Vec3d( 44.1941442048837, 15.3814535319104, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Antonio, US", osg::Vec3d( -98.4925343509243, 29.4849054770305, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Diego, US", osg::Vec3d( -117.089681287411, 32.7446268224376, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Francisco, US", osg::Vec3d( -122.441329206337, 37.7248410529552, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Jose, CS", osg::Vec3d( -84.0867257418277, 9.94013829320315, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Jose USA, US", osg::Vec3d( -121.856815068033, 37.3048386304369, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Paris, FR", osg::Vec3d( 2.36570869038777, 48.822809895984, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Shanghai, CH", osg::Vec3d( 121.459521026158, 31.2703392215796, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Skopje, MK", osg::Vec3d( 21.4318378123423, 42.009378412607, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Spokane, US", osg::Vec3d( -117.311416340812, 47.6748848185907, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "St Louis, US", osg::Vec3d( -90.3267411361066, 38.6463998651713, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "St Petersburg, RS", osg::Vec3d( 30.314664157316, 59.9427553404975, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Sydney, AS", osg::Vec3d( 150.988016679109, -33.8591838171749, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tallinn, EN", osg::Vec3d( 24.7676107037929, 59.4221584577344, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tashkent, UZ", osg::Vec3d( 69.2606121365834, 41.2802166253192, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tbilisi, GG", osg::Vec3d( 44.8412305215146, 41.7429883852999, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tehran, IR", osg::Vec3d( 51.4277725416965, 35.7043722952041, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "El Paso, US", osg::Vec3d( -106.367168362582, 31.789099094914, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Fairbanks, US", osg::Vec3d( -147.77182964056, 64.8310117988727, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Faisalabad, PK", osg::Vec3d( 73.0875940643739, 31.4118996365173, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Fortaleza, BR", osg::Vec3d( -38.5481302501578, -3.79182118867442, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Geneva, SZ", osg::Vec3d( 6.15615497328653, 46.2311369575368, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Guadalajara, MX", osg::Vec3d( -103.341170970159, 20.6630317907319, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Hamburg, GM", osg::Vec3d( 9.98166463893305, 53.5844417979861, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Hanoi, VM", osg::Vec3d( 105.834069509653, 21.0226581611267, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Harare, ZI", osg::Vec3d( 31.0549536955308, -17.8400523577712, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Havana, CU", osg::Vec3d( -82.3860323840276, 23.076005112852, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Helena, US", osg::Vec3d( -112.009752346258, 46.6151410428947, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Helsinki, FI", osg::Vec3d( 24.948709379188, 60.232965660646, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ho Chi Minh, VM", osg::Vec3d( 106.665662081234, 10.7829966618779, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Honolulu, US", osg::Vec3d( -157.867022226774, 21.3412572117872, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Houston, US", osg::Vec3d( -95.3638943532835, 29.78723358169, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Hyderabad, IN", osg::Vec3d( 78.4896012290094, 17.4278334115361, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ibadan, NI", osg::Vec3d( 3.90460761293311, 7.38883616598387, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Indianapolis, US", osg::Vec3d( -86.1586839564778, 39.8062242296974, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "San Salvador, ES", osg::Vec3d( -89.1936966356902, 13.7015528754452, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Santiago, CI", osg::Vec3d( -70.6501197478003, -33.477156027678, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Santo Domingo, DR", osg::Vec3d( -69.8952203137587, 18.4869941634304, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Sao Paolo, BR", osg::Vec3d( -46.5930844607256, -23.581045634523, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Saratov, RS", osg::Vec3d( 46.0420816279719, 51.5055486957339, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Saskatoon, CA", osg::Vec3d( -106.661116624076, 52.1451859650028, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Seattle, US", osg::Vec3d( -122.357838707611, 47.559957418901, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Jacksonville, US", osg::Vec3d( -81.5867424828215, 30.3140457252336, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Jeddah, SA", osg::Vec3d( 39.1936785903126, 21.5168019505006, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Johannesburg, SF", osg::Vec3d( 28.0847953376369, -26.2119624963641, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kabul, AF", osg::Vec3d( 69.1690547708298, 34.5280814955866, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kano, NI", osg::Vec3d( 8.52191373159353, 11.992740981993, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kansas City, US", osg::Vec3d( -94.5622938795558, 39.069561175595, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Karachi, PK", osg::Vec3d( 67.0667985878919, 24.8747602536013, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kazan, RS", osg::Vec3d( 49.1381253421573, 55.8056684179111, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kiev, UP", osg::Vec3d( 30.5191524366448, 50.4433947524901, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kinshasa, CF", osg::Vec3d( 15.2900084559153, -4.27310565828601, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Kirkuk, IZ", osg::Vec3d( 44.3772408855087, 35.4352071434591, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Lahore, PK", osg::Vec3d( 74.3314787365745, 31.534111837914, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "La Paz, BL", osg::Vec3d( -68.1501602884895, -16.5114679676872, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Las Vegas, US", osg::Vec3d( -115.137646080468, 36.1347645273855, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Lisbon, PO", osg::Vec3d( -9.1783639916852, 38.7720124748948, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "London, UK", osg::Vec3d( -0.11783575526748, 51.4997810580635, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Los Angeles, US", osg::Vec3d( -118.184319958992, 33.9562165411022, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Luanda, AO", osg::Vec3d( 13.2645088008277, -8.8274790612839, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Lusaka, ZA", osg::Vec3d( 28.3565162915976, -15.3908409863833, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Madrid, SP", osg::Vec3d( -3.67799408652979, 40.4072839241841, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Makkah, SA", osg::Vec3d( 39.8715721885327, 21.4293639689384, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Manila, RP", osg::Vec3d( 121.035524723664, 14.57523899765, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Maracaibo, VE", osg::Vec3d( -71.640273647793, 10.6353355172136, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Melbourne, AS", osg::Vec3d( 145.062629019551, -37.8744128346568, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Memphis, US", osg::Vec3d( -89.9714205132267, 35.1249952073896, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Mexico City, MX", osg::Vec3d( -99.0886383040833, 19.4400464605948, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Miami, US", osg::Vec3d( -80.2648725118507, 25.9819309114117, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Milwaukee, US", osg::Vec3d( -88.0626953188905, 43.0578351920068, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Minneapolis, US", osg::Vec3d( -93.2527806641115, 44.9207374656505, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Montevideo, UY", osg::Vec3d( -56.144857178683, -34.8636928125822, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Montreal, CA", osg::Vec3d( -73.7089186514456, 45.5594349409156, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Moscow, RS", osg::Vec3d( 37.6412608576027, 55.7688280185241, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Naimey, NG", osg::Vec3d( 2.14024515414936, 13.5167857311516, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Nairobi, KE", osg::Vec3d( 36.8471008657693, -1.29146101350431, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Naples, IT", osg::Vec3d( 14.2664422097936, 40.8970171972608, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Nashville, US", osg::Vec3d( -86.7609452295054, 36.1748047373206, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "New Orleans, US", osg::Vec3d( -90.0858384644025, 29.9490511348739, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "New York, US", osg::Vec3d( -73.9754957664963, 40.7491677207889, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Nouakchott, MR", osg::Vec3d( -15.9645830766092, 18.0895362540645, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Novosibirsk, RS", osg::Vec3d( 82.908594314613, 55.0379117552295, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Oakland, US", osg::Vec3d( -122.23169071825, 37.8025409744393, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Oklahoma City, US", osg::Vec3d( -97.5480202443281, 35.4955426669383, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Omaha, US", osg::Vec3d( -96.0085333410785, 41.2352868300022, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Orlando, US", osg::Vec3d( -81.3822702743953, 28.5519896864993, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ottawa, CA", osg::Vec3d( -75.670205304676, 45.4188335423195, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Perth, AS", osg::Vec3d( 115.873860692745, -31.9345175631841, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Philadelphia, US", osg::Vec3d( -75.1494898312549, 39.9968602498834, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Phnom Penh, CB", osg::Vec3d( 104.882748670325, 11.5625289482213, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Phoenix, US", osg::Vec3d( -112.011282442126, 33.4481548227444, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Pittsburgh, US", osg::Vec3d( -80.0232713142895, 40.4197407597333, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Portland, US", osg::Vec3d( -122.687046693423, 45.5303143475893, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Prague, EZ", osg::Vec3d( 14.4047708954144, 50.0717223155906, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Provo, US", osg::Vec3d( -111.718600757191, 40.2606082262247, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Pune, IN", osg::Vec3d( 73.862943875161, 18.5214088017687, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Basra, IZ", osg::Vec3d( 47.7503594341771, 30.5216880671759, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Beijing, CH", osg::Vec3d( 116.32725449758, 39.9131281439803, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Beirut, LE", osg::Vec3d( 35.5943415604018, 33.9273506103842, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Belfast, UK", osg::Vec3d( -5.92572669641833, 54.619084283678, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Berlin, GM", osg::Vec3d( 13.3951693079483, 52.504717257833, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bishkek, KG", osg::Vec3d( 74.6064138434135, 42.8764765510976, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bogota, CO", osg::Vec3d( -74.1086721727048, 4.63285850957035, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Boise, US", osg::Vec3d( -116.227946555485, 43.6067402394874, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bombay, IN", osg::Vec3d( 72.9102417915317, 19.0773606973281, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Boston, US", osg::Vec3d( -71.0588265045092, 42.3595387625327, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Brasilia, BR", osg::Vec3d( -47.8939416283784, -15.7857767665577, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Brisbane, AS", osg::Vec3d( 153.058201111771, -27.4294774345971, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Brussels, BE", osg::Vec3d( 4.38236543250254, 50.8624968453354, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bucharest, RO", osg::Vec3d( 26.1098294578468, 44.4421973182941, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Budapest, HU", osg::Vec3d( 19.1470904466789, 47.493945629673, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Buenos Aires, AR", osg::Vec3d( -58.5104620521919, -34.6205411975702, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Cairo, EG", osg::Vec3d( 31.2802975930989, 30.0602515090461, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Calcutta, IN", osg::Vec3d( 88.3116486004829, 22.657120113875, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Calgary, CA", osg::Vec3d( -114.078444373116, 51.0343586010632, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Canberra, AS", osg::Vec3d( 149.10264897015, -35.320556479969, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Cape Town, SF", osg::Vec3d( 18.5171538543167, -33.9830099663035, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Caracas, VE", osg::Vec3d( -66.8853969057375, 10.4724650358373, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Casablanca, MO", osg::Vec3d( -7.5779541217198, 33.5807089535346, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Charleston, US", osg::Vec3d( -79.9709126874107, 32.8130928762727, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Charlotte, US", osg::Vec3d( -80.8412365620596, 35.2257997609854, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Chicago, US", osg::Vec3d( -87.855775542989, 41.87029527433, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Cincinnati, US", osg::Vec3d( -84.4772543055487, 39.1984541274746, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Cleveland, US", osg::Vec3d( -81.6779284501518, 41.5028968584678, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Colombo, CE", osg::Vec3d( 79.8750665618516, 6.92000925668947, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Colorado Springs, US", osg::Vec3d( -104.796798758206, 38.8403232418333, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Copenhagen, DA", osg::Vec3d( 12.4708946447833, 55.7300072589713, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Dallas, US", osg::Vec3d( -96.921006406714, 32.9408467692849, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Damascus, SY", osg::Vec3d( 36.3439970508776, 33.5140628561274, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Dar Es Salaam, TZ", osg::Vec3d( 39.2649552670369, -6.80786639164302, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Delhi, IN", osg::Vec3d( 77.1815991702045, 28.625471447924, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Denver, US", osg::Vec3d( -104.964736895042, 39.795546425831, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Detroit, US", osg::Vec3d( -83.1545972680673, 42.3890847600183, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Dhaka, BG", osg::Vec3d( 90.413491138569, 23.8003827680532, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Dublin, EI", osg::Vec3d( -6.23143868979274, 53.3332620472379, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Durban, SF", osg::Vec3d( 30.9433569965427, -29.8709096161508, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Edmonton, CA", osg::Vec3d( -113.498943998261, 53.5387301190822, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tokyo, JA", osg::Vec3d( 139.742121939157, 35.6719042040691, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Torino, IT", osg::Vec3d( 7.6436510491676, 45.0690115312893, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Toronto, CA", osg::Vec3d( -79.4825404931527, 43.6494913091285, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tripoli, LY", osg::Vec3d( 13.2015232696729, 32.8666670529069, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tucson, US", osg::Vec3d( -110.916707027548, 32.2438145109246, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Tunis, TS", osg::Vec3d( 10.1797070832528, 36.8128209152987, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ulan Bator, MG", osg::Vec3d( 106.880727455533, 47.8836754211534, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Vancouver, CA", osg::Vec3d( -123.011675395686, 49.2260430288262, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Victoria, CA", osg::Vec3d( -123.398415333749, 48.4626506537767, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Warsaw, PL", osg::Vec3d( 20.9493507853338, 52.2305409782089, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Windhoek, WA", osg::Vec3d( 17.0599934390026, -22.560957136383, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Winnipeg, CA", osg::Vec3d( -97.1483968495162, 49.869088095289, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Yekaterinburg, RS", osg::Vec3d( 60.5869115028861, 56.8291525129405, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Yellowstone, US", osg::Vec3d( -110.497507890748, 44.6160641838463, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Yeravan, AM", osg::Vec3d( 44.5111631853673, 40.17292402771, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Belgrade, SR", osg::Vec3d( 20.4626595963562, 44.8037769293934, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Belo Horizonte, BR", osg::Vec3d( -44.0512930106878, -19.8884364346431, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Abu Dhabi, TC", osg::Vec3d( 54.3920904944833, 24.4677407120568, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Addis Ababa, ET", osg::Vec3d( 38.747736075642, 9.00836315449831, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Aguas Calientes, MX", osg::Vec3d( -102.279040792839, 21.8803861097451, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ahmendabad, IN", osg::Vec3d( 72.5852463608633, 23.0315526907318, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Albuquerque, US", osg::Vec3d( -106.608622821288, 35.1132351967383, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Alexandria, EG", osg::Vec3d( 29.8802672641438, 31.1846010606836, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Amman, JO", osg::Vec3d( 35.9203471204005, 31.9393266354332, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Amsterdam, NL", osg::Vec3d( 4.8687721981823, 52.3964017690447, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Anchorage, US", osg::Vec3d( -149.857119193753, 61.1766014889288, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ankara, TU", osg::Vec3d( 32.8088552754561, 39.9360056758901, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Ashkhabad, TX", osg::Vec3d( 58.3805684313491, 37.9445820874411, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Asmara, ER", osg::Vec3d( 38.9133734604065, 15.3337981650915, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Asuncion, PA", osg::Vec3d( -57.5972646855721, -25.306423140337, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Athens, GR", osg::Vec3d( 23.7045096920646, 37.9791516495346, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Atlanta, US", osg::Vec3d( -84.3915383973462, 33.810787746719, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Auckland, NZ", osg::Vec3d( 174.772891842425, -36.87022970525, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Austin, US", osg::Vec3d( -97.7178006678997, 30.3087289007672, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Baghdad, IZ", osg::Vec3d( 44.3954097848764, 33.316913063281, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Baku, AJ", osg::Vec3d( 49.8934197045622, 40.3961940474947, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Baltimore, US", osg::Vec3d( -76.6125776611219, 39.2884454502798, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bamako, ML", osg::Vec3d( -7.9952173477088, 12.6451040151871, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bangalore, IN", osg::Vec3d( 77.594781741464, 12.9735913032004, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Bangkok, TH", osg::Vec3d( 100.56035054686, 13.7783496561836, 0 ), 0.0, -90, 1e4 ),
osgEarth::Viewpoint( "Barcelona, SP", osg::Vec3d( 2.13246973992958, 41.3645146681043, 0 ), 0.0, -90, 1e4 ),
};
