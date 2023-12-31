import React, { useState } from "react";
import { Modal, Form, Input, Button } from "antd";
import axios from "axios";
import {
  APIURL,
  currentTimestampInSeconds,
  generateRandomSixDigitNumber,
} from "../APIS/Constant";

const NewPost = ({ visible, onClose }) => {
  const [form] = Form.useForm();
  const [loading, setLoading] = useState(false);

  const handleOk = async () => {
    try {
      setLoading(true);
      const values = await form.validateFields();
      const user = JSON.parse(localStorage.getItem("user")).user;
      axios
        .post(
          APIURL,
          generateRandomSixDigitNumber() +
            "`addVE`posts`0`" +
            user +
            "`" +
            user +
            currentTimestampInSeconds +
            "`cat`post`desc:" +
            values.description +
            "~timestamp:" +
            currentTimestampInSeconds +
            "~`"
        )
        .then((res) => {
          if (res.data.toString() === "Success: Vertex added") {
            alert("Posted Successfully");
          } else {
            alert("Someting went wrong");
          }
        })
        .catch((err) => {
          alert("Someting went wrong");
        });
      form.resetFields();
      onClose();
    } catch (error) {
      console.error("Validation Error:", error);
    } finally {
      setLoading(false);
    }
  };

  const handleCancel = () => {
    form.resetFields();
    onClose();
  };

  return (
    <Modal
      visible={visible}
      title="Whats on your mind"
      onOk={handleOk}
      onCancel={handleCancel}
      footer={[
        <Button key="cancel" className='btnClass' onClick={handleCancel}>
          Cancel
        </Button>,
        <Button
          key="submit"
          type="primary"
          loading={loading}
          onClick={handleOk}
          className='btnClass'
        >
          Post
        </Button>,
      ]}
    >
      <Form form={form} layout="vertical">
        <Form.Item
          name="description"
          label=""
          rules={[
            { required: true, message: "Please enter a description" },
            { max: 255, message: "Description cannot exceed 255 characters" },
          ]}
        >
          <Input.TextArea rows={4} placeholder="Whats on your mind" />
        </Form.Item>
      </Form>
    </Modal>
  );
};

export default NewPost;
