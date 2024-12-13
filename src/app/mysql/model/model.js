module.exports = (sequelizeConfig, Sequelize) => {
    const database = sequelizeConfig.define(
        'database',
        {
            id: {
                type: Sequelize.INTEGER,
                primaryKey: true,
                autoIncrement: true,
            },
            count: {
                type: Sequelize.BIGINT,
            },
            disabledType: {
                type: Sequelize.STRING,
            },
        }
    );
    return database;
};

//https://pjt3591oo.github.io/sequelizejs_translate/build/html/CoreConcepts/DateTypes.html