const router = require('express').Router();
const methods = require('../controller/controller.js');

router.post('/api/enroll',methods.enroll);
router.post('/api/busstop',methods.stop);
router.get('/api/bus/:busId',methods.bus);
router.get('/api/info',methods.info);
//router.post('/api/register',tutorial.register);

module.exports = router;
