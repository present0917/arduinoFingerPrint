module.exports = {
    host: 'localhost',
    username: 'root',
    password: '0329',
    db: 'fortest',
    dialect: 'mysql',
    pool: {
        max: 5,
        min: 0,
        acquire: 30000,
        idle: 10000
    }
};
