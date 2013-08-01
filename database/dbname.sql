
DROP TABLE IF EXISTS `temperatures`;

CREATE TABLE `temperatures` (
  `Prefix` varchar(255) DEFAULT NULL,
  `Date` varchar(255) DEFAULT NULL,
  `Temperature` varchar(255) DEFAULT NULL
  `Status` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

