import React, { useEffect, useState } from "react";
import { Layout, Menu } from "antd";
import { LogoutOutlined, UserOutlined } from "@ant-design/icons";
import { useNavigate } from "react-router-dom";

const { Header } = Layout;

const TopBar = () => {
  const navigate = useNavigate();
  const [username, setUsername] = useState("Guest");
  useEffect(() => {
    if (localStorage.getItem("user")) {
      const item = localStorage.getItem("user");
      const data = JSON.parse(item);
      setUsername(data?.user);
    } else {
      setUsername("Guest");
    }
  }, []);
  const onLogout = async () => {
    localStorage.clear("user");
    window.location.reload();
  };
  return (
    <Header
      style={{
        display: "flex",
        justifyContent: "space-between",
        alignItems: "center",
        background: "#242015"
      }}
    >
      <div
        style={{ color: "#ebe0bb" }}
        onClick={() => {
          navigate("/posts");
        }}
      >
        Silly Cat Network
      </div>
      {username && (
        <span style={{ color: "#ebe0bb" }}>
          <UserOutlined style={{ marginRight: 8, color: "#ebe0bb" }} />
          {username}
        </span>
      )}
      <LogoutOutlined
        title="Logout"
        key="2"
        onClick={onLogout}
        style={{ color: "#ebe0bb" }}
      ></LogoutOutlined>
    </Header>
  );
};

export default TopBar;
