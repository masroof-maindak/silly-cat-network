import React from 'react';
import { Form, Input, Button, DatePicker, Typography } from 'antd';
import axios from 'axios';
import { APIURL, generateRandomSixDigitNumber } from '../APIS/Constant';

const { Title } = Typography;

const SignUp = ({ isSignUp }) => {
  // values = json object of form data
  const onFinish = (values) => {
    console.log('Received values:', values);

    axios
      .post(
        APIURL,
        //send addVertex call to server
        `${generateRandomSixDigitNumber()}\`addVertex\`${values.username}\`cat\`username:${values.username}~!password:${values.password}~dob:${values.dob}~\``
      )
      .then((res) => {
        if ((res.data).toString() === "Success: Vertex added") {
          alert("Account Created Successfully! Please Login to Continue");
        } else {
          alert("User Already Exists")
        }
      })
      .catch((err) => {
        alert("User Already Exists");
      });
  };

  return (
    <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100vh' }}>
      <div>
        <Title level={2}
          style={{ color: "#ebe0bb", textAlign: "center", fontFamily: "Maple Mono" }}
        >
          Sign Up
        </Title>
        <Form
          name="signupForm"
          onFinish={onFinish}
          initialValues={{ remember: true }}
          style={{ maxWidth: '300px' }}
        >
          <Form.Item
            name="username"
            rules={[{ required: true, message: 'Please enter your username!' }]}
          >
            <Input placeholder="Username" className="swamp-form" />
          </Form.Item>

          <Form.Item
            name="nickname"
            rules={[{ required: true, message: 'Please enter your nickname!' }]}
          >
            <Input placeholder="Nickname" className="swamp-form" />
          </Form.Item>

          <Form.Item
            name="password"
            rules={[{ required: true, message: 'Please enter your password!' }]}
          >
            <Input.Password placeholder="Password" className="swamp-form"/>
          </Form.Item>

          <Form.Item
            name="confirmPassword"
            dependencies={['password']}
            rules={[
              { required: true, message: 'Please confirm your password!' },
              ({ getFieldValue }) => ({
                validator(_, value) {
                  if (!value || getFieldValue('password') === value) {
                    return Promise.resolve();
                  }
                  return Promise.reject(new Error('The two passwords do not match!'));
                },
              }),
            ]}
          >
            <Input.Password placeholder="Confirm Password" className="swamp-form"/>
          </Form.Item>

          <Form.Item
            name="dob"
            rules={[{ required: true, message: 'Please select your date of birth!' }]}
          >
            <DatePicker placeholder="Date of Birth" className="swamp-form" style={{ width: '100%' }} />
          </Form.Item>

          <Form.Item>
            <Button type="primary" htmlType="submit" style={{ width: '100%', backgroundColor: "#db930d" }}>
              <span
                style={{ background: "#db930d", color: "#242015" }}
              >
                Ready for Whimsical Antics
              </span>
            </Button>
          </Form.Item>
        </Form>

        <div style={{ marginTop: '10px', textAlign: 'center' }}>
          <span>Already have an account?</span>
          <Button type="link" onClick={() => isSignUp(false)}>
            <span
              style={{ color: "#c1666b", textDecoration: "underline" }}
            >
              Login
            </span>
          </Button>
        </div>
      </div>
    </div>
  );
};

export default SignUp;
