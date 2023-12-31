import React from "react";
import { Form, Input, Button, Typography } from "antd";
import { useNavigate } from "react-router-dom";
import axios from "axios";
import { APIURL, convertStringToJson, generateRandomSixDigitNumber } from "../APIS/Constant";

const { Title } = Typography;

const Login = ({ isSignUp }) => {
  const navigate = useNavigate();
  const onFinish = async (values) => {
    axios
      .post(
        APIURL,
        // pull client data from server to check if user exists + password is correct
        generateRandomSixDigitNumber() + "`fetchVertexProperties`" + values.username + "`cat`"
      )
      .then((res) => {
        const data = convertStringToJson(res.data);
        console.log(data);
        if (data._password == values.password) {
          const data = {
            user: values.username,
            isAuthenticated: true,
          };
          localStorage.setItem("user", JSON.stringify(data));
          navigate("/posts");
          window.location.reload();
        } else {
          alert("Wrong Username and password");
        }
      })
      .catch((err) => {
        alert("Wrong Username and password");
      });
  };

  return (
    <div
      style={{
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        height: "100vh",
      }}
    >
      <div>
        <Title level={2}
          style={{ color: "#ebe0bb", textAlign: "center", fontFamily: "Maple Mono" }}
        >
          Login
        </Title>
        <Form
          name="loginForm"
          onFinish={onFinish}
          initialValues={{ remember: true }}
          style={{ maxWidth: "300px" }}
        >
          <Form.Item
            name="username"
            rules={[{ required: true, message: "Please enter your username!" }]}
            style={{ color: "red" }}
          >
            <Input placeholder="Username" className="swamp-form"/>
          </Form.Item>

          <Form.Item
            name="password"
            rules={[{ required: true, message: "Please enter your password!" }]}
          >
            <Input.Password className="swamp-form" placeholder="Password" />
          </Form.Item>

          <Form.Item>
            <Button type="primary" htmlType="submit" style={{ width: "100%", background: "#db930d" }}>
              <span
                style={{ background: "#db930d", color: "#242015" }}
              >
                Get Silly?
              </span>
            </Button>
          </Form.Item>
        </Form>

        <div style={{ marginTop: "10px", textAlign: "center" }}>
          <span>Don't have an account?</span>
          <Button type="link" onClick={() => isSignUp(true)}>
            <span
              style={{ color: "#c1666b", textDecoration: "underline" }}
            >
              Sign Up
            </span>
          </Button>
        </div>
      </div>
    </div>
  );
};

export default Login;
