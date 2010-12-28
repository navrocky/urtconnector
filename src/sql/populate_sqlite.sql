CREATE TABLE IF NOT EXISTS all_state
(
    id INTEGER,
    address VARCHAR(255),
    name VARCHAR(255),
    gametype INTEGER,
    map VARCHAR(255),
    mapurl VARCHAR(255),
    maxplayercount VARCHAR(255),
    mode INTEGER,
    ping INTEGER,
    country VARCHAR(255),
    countrycode VARCHAR(255),
    info LONGTEXT,
    PRIMARY KEY(id)
);
CREATE TABLE IF NOT EXISTS favs_state
(
    id INTEGER,
    address VARCHAR(255),
    name VARCHAR(255),
    gametype INTEGER,
    map VARCHAR(255),
    mapurl VARCHAR(255),
    maxplayercount VARCHAR(255),
    mode INTEGER,
    ping INTEGER,
    country VARCHAR(255),
    countrycode VARCHAR(255),
    info LONGTEXT,
    PRIMARY KEY(id)
);
